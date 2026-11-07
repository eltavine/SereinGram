#pragma once

#include "base/basic_types.h"
#include "serein/features/inspector/model/tl_tree.h"

#include <QtCore/QByteArray>

#include <gsl/pointers>

#include <optional>
#include <utility>
#include <vector>

class HistoryItem;
class PeerData;

namespace MTP {
class Sender;
} // namespace MTP

namespace Serein::Inspector {

enum class Origin {
	Server,
	Saved,
	Missing,
	Failed,
};

struct Snapshot {
	std::optional<Node> root;
	Origin origin = Origin::Missing;
	QString error;
};

using ExtraFacts = std::vector<std::pair<QString, QString>>;
using SavedProvider = Fn<QByteArray(gsl::not_null<const HistoryItem*>)>;
using FactsProvider = Fn<ExtraFacts(gsl::not_null<HistoryItem*>)>;

void SetSavedProvider(SavedProvider provider);
void AddFactsProvider(FactsProvider provider);
[[nodiscard]] ExtraFacts CollectExtraFacts(gsl::not_null<HistoryItem*> item);

void LoadMessage(
	gsl::not_null<HistoryItem*> item,
	MTP::Sender &api,
	Fn<void(Snapshot)> done);
void LoadPeer(
	gsl::not_null<PeerData*> peer,
	MTP::Sender &api,
	Fn<void(Snapshot)> done);

} // namespace Serein::Inspector
