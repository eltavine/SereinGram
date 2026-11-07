#pragma once

namespace Ui { class InputField; }
namespace Window { class SessionController; }

namespace Serein::Chats {

void AttachChatListTools(
	gsl::not_null<Ui::InputField*> field,
	gsl::not_null<Window::SessionController*> controller);

} // namespace Serein::Chats
