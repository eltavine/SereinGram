#pragma once

namespace Serein::Menu {

enum class ActionId : int {
	Reply = 1,
	Edit,
	Copy,
	CopyLink,
	Forward,
	Translate,
	Pin,
	Select,
	Statistics,
	Report,
	BlockSender,
	Image,
	Delete,
	StickerPack,
	Repeat,
	RepeatAsCopy,
	ForwardWithoutQuote,
	Batch,
	SelectSender,
	MediaInfo,
	Screenshot = 21,
	Reading = 22,
	FilterAuthor = 23,
	EditHistory = 24,
	DeletedMessages = 25,
	ReadUntilHere = 26,
	HistoryExclusion = 27,
	ButtonData = 28,
	MessageDetails = 29,
	SelectRange = 30,
};

} // namespace Serein::Menu
