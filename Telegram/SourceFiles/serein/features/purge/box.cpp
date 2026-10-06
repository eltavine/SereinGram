#include "serein/features/purge/box.h"

#include "serein/features/purge/gateway.h"
#include "serein/features/purge/model/job.h"
#include "serein/features/purge/model/plan.h"
#include "base/flat_set.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/send_as_peers.h"
#include "main/session/session_show.h"
#include "ui/boxes/choose_date_time.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_serein.h"

#include <QtCore/QLocale>
#include <QtCore/QPointer>

namespace Serein::Purge {
namespace {

constexpr auto kCounting = -2;
constexpr auto kEarliestDate = TimeId(1375315200);

struct State {
	Identities identities;
	std::vector<Ui::Checkbox*> checks;
	std::vector<int> counts;
	Age age = Age::All;
	TimeId custom = 0;
	int generation = 0;
};

[[nodiscard]] QString NameOf(gsl::not_null<PeerData*> identity) {
	return identity->isSelf()
		? tr::lng_serein_delete_mine_account(
			tr::now,
			lt_name,
			identity->name())
		: identity->name();
}

[[nodiscard]] QString RowText(
		gsl::not_null<PeerData*> identity,
		int count) {
	const auto name = NameOf(identity);
	if (count == kCounting) {
		return name
			+ u" · "_q
			+ tr::lng_serein_delete_mine_counting(tr::now);
	} else if (count < 0) {
		return name;
	}
	return name + u" · "_q + QString::number(count);
}

[[nodiscard]] std::vector<int> Picked(const State &state) {
	auto result = std::vector<int>();
	for (auto i = 0; i != int(state.checks.size()); ++i) {
		if (state.checks[i]->checked()) {
			result.push_back(i);
		}
	}
	return result;
}

[[nodiscard]] int Known(const State &state, const std::vector<int> &picked) {
	auto result = 0;
	for (const auto index : picked) {
		result += std::max(state.counts[index], 0);
	}
	return result;
}

[[nodiscard]] bool Counting(
		const State &state,
		const std::vector<int> &picked) {
	return ranges::any_of(picked, [&](int index) {
		return state.counts[index] == kCounting;
	});
}

[[nodiscard]] QString ProgressText(Progress progress) {
	return tr::lng_serein_delete_mine_progress(
		tr::now,
		lt_done,
		QString::number(progress.deleted),
		lt_total,
		QString::number(progress.total));
}

[[nodiscard]] QString ResultText(const Result &result) {
	const auto amount = QString::number(result.deleted);
	switch (result.outcome) {
	case Outcome::Done:
		return result.deleted
			? tr::lng_serein_delete_mine_done(tr::now, lt_amount, amount)
			: tr::lng_serein_delete_mine_none(tr::now);
	case Outcome::Stopped:
		return tr::lng_serein_delete_mine_stopped(
			tr::now,
			lt_amount,
			amount);
	case Outcome::Failed:
		break;
	}
	return tr::lng_serein_delete_mine_failed(
		tr::now,
		lt_error,
		result.error,
		lt_amount,
		amount);
}

void RunBox(
		not_null<Ui::GenericBox*> box,
		std::shared_ptr<Main::SessionShow> show,
		std::shared_ptr<Job> job,
		int total) {
	box->setTitle(tr::lng_serein_delete_mine_progress_title());
	box->setCloseByEscape(false);
	box->setCloseByOutsideClick(false);
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		ProgressText({ .total = total }),
		st::boxLabel));
	box->addButton(tr::lng_serein_delete_mine_stop(), [=] {
		job->stop();
	});
	job->start(crl::guard(box, [=](Progress progress) {
		label->setText(ProgressText(progress));
	}), crl::guard(box, [=](Result result) {
		show->showToast(ResultText(result));
		box->closeBox();
	}));
}

void OptionsBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> chat) {
	box->setTitle(tr::lng_serein_delete_mine());
	box->setWidth(st::boxWideWidth);
	const auto state = box->lifetime().make_state<State>();
	const auto content = box->verticalLayout();
	Ui::AddSubsectionTitle(content, tr::lng_serein_delete_mine_whose());
	const auto rows = content->add(object_ptr<Ui::VerticalLayout>(content));
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, tr::lng_serein_delete_mine_age());
	const auto group = std::make_shared<Ui::RadioenumGroup<Age>>(Age::All);
	const auto addAge = [&](Age age, const QString &text) {
		return content->add(
			object_ptr<Ui::Radioenum<Age>>(
				content,
				group,
				age,
				text,
				st::defaultBoxCheckbox),
			st::sereinPurgeOptionPadding);
	};
	addAge(Age::All, tr::lng_serein_delete_mine_all(tr::now));
	addAge(Age::Day, tr::lng_serein_delete_mine_day(tr::now));
	addAge(Age::Week, tr::lng_serein_delete_mine_week(tr::now));
	addAge(Age::Month, tr::lng_serein_delete_mine_month(tr::now));
	addAge(Age::Year, tr::lng_serein_delete_mine_year(tr::now));
	const auto custom = addAge(
		Age::Custom,
		tr::lng_serein_delete_mine_before(tr::now));
	Ui::AddSkip(content);
	const auto summary = content->add(
		object_ptr<Ui::FlatLabel>(content, QString(), st::boxLabel),
		st::boxRowPadding);

	const auto updateSummary = [=] {
		const auto picked = Picked(*state);
		summary->setText(picked.empty()
			? tr::lng_serein_delete_mine_pick(tr::now)
			: Counting(*state, picked)
			? tr::lng_serein_delete_mine_counting(tr::now)
			: tr::lng_serein_delete_mine_summary(
				tr::now,
				lt_amount,
				QString::number(Known(*state, picked))));
	};
	const auto updateRow = [=](int index) {
		state->checks[index]->setText(RowText(
			state->identities[index],
			state->counts[index]));
	};
	const auto recount = [=] {
		const auto generation = ++state->generation;
		const auto before = Cutoff(
			state->age,
			base::unixtime::now(),
			state->custom);
		for (auto i = 0; i != int(state->identities.size()); ++i) {
			state->counts[i] = kCounting;
			updateRow(i);
			CountMessages(
				chat,
				state->identities[i],
				before,
				crl::guard(box, [=](int count) {
					if (state->generation == generation) {
						state->counts[i] = count;
						updateRow(i);
						updateSummary();
					}
				}));
		}
		updateSummary();
	};
	const auto rebuild = [=] {
		const auto first = state->checks.empty();
		auto kept = base::flat_set<PeerId>();
		for (auto i = 0; i != int(state->checks.size()); ++i) {
			if (state->checks[i]->checked()) {
				kept.emplace(state->identities[i]->id);
			}
		}
		rows->clear();
		state->checks.clear();
		state->identities = IdentitiesFor(chat);
		state->counts.assign(state->identities.size(), kCounting);
		for (const auto &identity : state->identities) {
			const auto check = rows->add(
				object_ptr<Ui::Checkbox>(
					rows,
					NameOf(identity),
					first ? identity->isSelf() : kept.contains(identity->id),
					st::defaultBoxCheckbox),
				st::sereinPurgeOptionPadding);
			check->checkedChanges() | rpl::on_next([=](bool) {
				updateSummary();
			}, check->lifetime());
			state->checks.push_back(check);
		}
		recount();
	};
	const auto chooseDate = [=] {
		const auto chosen = std::make_shared<bool>(false);
		controller->show(Box([=](not_null<Ui::GenericBox*> dates) {
			Ui::ChooseDateTimeBox(dates, {
				.title = tr::lng_serein_delete_mine_choose(),
				.submit = tr::lng_box_ok(),
				.done = crl::guard(box, [=](TimeId date) {
					*chosen = true;
					state->custom = date;
					state->age = Age::Custom;
					custom->setText(tr::lng_serein_delete_mine_before_date(
						tr::now,
						lt_date,
						QLocale().toString(
							base::unixtime::parse(date),
							QLocale::ShortFormat)));
					recount();
					dates->closeBox();
				}),
				.min = [] { return kEarliestDate; },
				.time = state->custom ? state->custom : base::unixtime::now(),
				.max = [] { return base::unixtime::now(); },
			});
			dates->boxClosing() | rpl::on_next(crl::guard(box, [=] {
				if (!*chosen) {
					group->setValue(state->age);
				}
			}), dates->lifetime());
		}), Ui::LayerOption::KeepOther);
	};
	group->setChangedCallback([=](Age age) {
		if (age == Age::Custom) {
			chooseDate();
		} else {
			state->age = age;
			recount();
		}
	});

	const auto key = Main::SendAsKey(chat);
	auto &sendAs = chat->session().sendAsPeers();
	sendAs.updated() | rpl::filter([=](const Main::SendAsKey &updated) {
		return updated == key;
	}) | rpl::on_next([=](const Main::SendAsKey &) {
		rebuild();
	}, box->lifetime());
	sendAs.refresh(key);
	rebuild();

	box->addButton(tr::lng_box_delete(), [=] {
		const auto picked = Picked(*state);
		if (picked.empty()) {
			box->showToast(tr::lng_serein_delete_mine_pick(tr::now));
			return;
		}
		const auto total = Known(*state, picked);
		const auto before = Cutoff(
			state->age,
			base::unixtime::now(),
			state->custom);
		const auto text = total
			? tr::lng_serein_delete_mine_confirm(
				tr::now,
				lt_amount,
				QString::number(total),
				lt_chat,
				chat->name())
			: tr::lng_serein_delete_mine_sure(
				tr::now,
				lt_chat,
				chat->name());
		const auto show = controller->uiShow();
		const auto identities = state->identities;
		const auto weak = QPointer<Ui::GenericBox>(box.get());
		controller->show(Ui::MakeConfirmBox({
			.text = text,
			.confirmed = [=](Fn<void()> &&close) {
				close();
				if (weak) {
					weak->closeBox();
				}
				const auto job = std::make_shared<Job>(
					MakeGateway(chat, identities),
					picked,
					before,
					total);
				show->showBox(Box(RunBox, show, job, total));
			},
			.confirmText = tr::lng_box_delete(),
			.confirmStyle = &st::attentionBoxButton,
		}), Ui::LayerOption::KeepOther);
	}, st::attentionBoxButton);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

bool CanDeleteMine(gsl::not_null<PeerData*> peer) {
	return peer->isChat() || peer->isMegagroup();
}

void ShowDeleteMine(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> chat) {
	controller->show(Box(OptionsBox, controller.get(), chat.get()));
}

} // namespace Serein::Purge
