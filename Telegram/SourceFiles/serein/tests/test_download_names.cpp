#include "serein/media/download_names.h"

#include "base/basic_types.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestDownloadNames() {
	using Serein::Media::DownloadFolderName;
	Require(DownloadFolderName(u"News & Talk"_q, 1) == u"News & Talk"_q,
		"plain chat name changed");
	Require(DownloadFolderName(u"a/b\\c:d*e?f\"g<h>i|j"_q, 1)
		== u"a_b_c_d_e_f_g_h_i_j"_q, "forbidden characters kept");
	Require(DownloadFolderName(u"tab\there"_q, 1) == u"tab_here"_q,
		"control characters kept");
	Require(DownloadFolderName(u"  dots... "_q, 1) == u"dots"_q,
		"trailing dots or blanks kept");
	Require(DownloadFolderName(u"con"_q, 1) == u"con_"_q
		&& DownloadFolderName(u"LPT1.txt"_q, 1) == u"LPT1.txt_"_q
		&& DownloadFolderName(u"Console"_q, 1) == u"Console"_q,
		"reserved device names");
	Require(DownloadFolderName(QString(), 777) == u"777"_q
		&& DownloadFolderName(u"..."_q, 778) == u"778"_q,
		"empty names do not fall back to the id");
	Require(DownloadFolderName(QString(100, u'x'), 1).size() == 64,
		"long names are not truncated");
}
