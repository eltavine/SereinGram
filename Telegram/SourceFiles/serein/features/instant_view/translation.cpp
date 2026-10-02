#include "serein/features/instant_view/translation.h"

#include "apiwrap.h"
#include "base/algorithm.h"
#include "iv/iv_rich_message_serializer.h"
#include "iv/iv_rich_page.h"
#include "main/main_session.h"

#include <optional>
#include <vector>

namespace Serein::IvTranslation {
namespace {

using Page = ::Iv::RichPage;
using Block = ::Iv::RichPage::Block;

struct Chunk {
	std::vector<Block> blocks;
	std::optional<MTPInputRichMessage> input;
	std::shared_ptr<const Page> translated;
};

struct State {
	not_null<Main::Session*> session;
	std::shared_ptr<const Page> source;
	LanguageId to;
	std::vector<Chunk> chunks;
	Fn<void(Result result)> done;
	QString error;
};

[[nodiscard]] Page WithBlocks(const Page &page, std::vector<Block> blocks) {
	return {
		.url = page.url,
		.rtl = page.rtl,
		.part = page.part,
		.views = page.views,
		.blocks = std::move(blocks),
	};
}

[[nodiscard]] std::optional<MTPInputRichMessage> Serialize(
		not_null<Main::Session*> session,
		const Page &page) {
	auto result = ::Iv::SerializeInputRichMessage(
		session,
		page,
		::Iv::SerializeInputRichMessageMode::FinalSubmit);
	return (result.status == ::Iv::SerializeInputRichMessageStatus::Success)
		? std::move(result.value)
		: std::nullopt;
}

[[nodiscard]] std::vector<Chunk> Split(
		not_null<Main::Session*> session,
		const Page &page) {
	const auto limits = ::Iv::ResolveRichMessageLimits(session);
	auto result = std::vector<Chunk>();
	auto pending = WithBlocks(page, {});
	const auto flush = [&] {
		if (pending.blocks.empty()) {
			return;
		}
		auto input = Serialize(session, pending);
		result.push_back({
			.blocks = base::take(pending.blocks),
			.input = std::move(input),
		});
	};
	for (const auto &block : page.blocks) {
		const auto alone = WithBlocks(page, { block });
		if (::Iv::ValidateRichMessage(alone, limits)
			|| !Serialize(session, alone)) {
			flush();
			result.push_back({ .blocks = { block } });
			continue;
		}
		pending.blocks.push_back(block);
		if (::Iv::ValidateRichMessage(pending, limits)) {
			pending.blocks.pop_back();
			flush();
			pending.blocks.push_back(block);
		}
	}
	flush();
	return result;
}

void Finish(const std::shared_ptr<State> &state) {
	auto blocks = std::vector<Block>();
	auto rtl = std::optional<bool>();
	for (const auto &chunk : state->chunks) {
		const auto &from = chunk.translated
			? chunk.translated->blocks
			: chunk.blocks;
		if (chunk.translated && !rtl) {
			rtl = chunk.translated->rtl;
		}
		blocks.insert(end(blocks), from.begin(), from.end());
	}
	auto result = Result{ .error = state->error };
	if (rtl) {
		auto page = WithBlocks(*state->source, std::move(blocks));
		page.rtl = *rtl;
		result.page = std::make_shared<const Page>(std::move(page));
	} else if (result.error.isEmpty()) {
		result.error = u"NOTHING_TRANSLATED"_q;
	}
	state->done(std::move(result));
}

void SendNext(const std::shared_ptr<State> &state, std::size_t index) {
	while (index < state->chunks.size() && !state->chunks[index].input) {
		++index;
	}
	if (index == state->chunks.size()) {
		Finish(state);
		return;
	}
	using Flag = MTPmessages_TranslateRichMessage::Flag;
	const auto session = state->session;
	session->api().request(MTPmessages_TranslateRichMessage(
		MTP_flags(Flag::f_text),
		MTPInputPeer(),
		MTPVector<MTPint>(),
		MTP_vector<MTPInputRichMessage>(1, *state->chunks[index].input),
		MTP_string(state->to.twoLetterCode()),
		MTPstring()
	)).done([=](const MTPmessages_TranslatedRichMessage &result) {
		const auto &list = result.data().vresult().v;
		if (!list.isEmpty()) {
			state->chunks[index].translated = ::Iv::ParseRichPage(
				session,
				list.front());
		}
		SendNext(state, index + 1);
	}).fail([=](const MTP::Error &error) {
		state->error = error.type();
		Finish(state);
	}).send();
}

} // namespace

void TranslatePage(
		not_null<Main::Session*> session,
		std::shared_ptr<const ::Iv::RichPage> page,
		LanguageId to,
		Fn<void(Result result)> done) {
	Expects(page != nullptr);

	auto chunks = Split(session, *page);
	SendNext(std::make_shared<State>(State{
		.session = session,
		.source = std::move(page),
		.to = to,
		.chunks = std::move(chunks),
		.done = std::move(done),
	}), 0);
}

} // namespace Serein::IvTranslation
