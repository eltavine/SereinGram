#include "serein/settings/service_editor.h"

#include "core/application.h"
#include "core/file_utilities.h"
#include "lang/lang_keys.h"
#include "serein/services/credentials.h"
#include "serein/services/request.h"
#include "serein/services/translation_protocol.h"
#include "settings/settings_common_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/fields/password_input.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QUuid>

#include <array>

#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

bool CredentialUsed(const ServicesConfig &config, const QString &account) {
	for (const auto &instance : config.instances) {
		const auto service = ParseService(instance);
		if (service && CredentialAccount(*service) == account) {
			return true;
		}
	}
	return false;
}

CredentialError RemoveServiceCredential(const ServiceDefinition &service) {
	const auto account = CredentialAccount(service);
	if (!service.useKey && ReadCredential(account).error == CredentialError::Unavailable) {
		return CredentialError::None;
	}
	return DeleteCredential(account);
}

void ServiceTestBox(
		not_null<Ui::GenericBox*> box,
		ServiceDefinition service,
		Fn<void(QString)> chooseModel) {
	box->setTitle(tr::lng_serein_service_test());

	box->addRow(object_ptr<Ui::FlatLabel>(box, rpl::single(
		service.name + u"\n"_q + ServiceEndpoint(service).toDisplayString()
		+ u"\n"_q + tr::lng_serein_service_test_about(tr::now)), st::boxLabel));
	const auto result = box->addRow(object_ptr<Ui::FlatLabel>(
		box, rpl::single(QString()), st::boxLabel));
	result->setSelectable(true);
	struct State {
		ServiceRequest request;
		int generation = 0;
	};
	const auto state = box->lifetime().make_state<State>();
	const auto request = &state->request;
	const auto generation = &state->generation;
	const auto modelRows = box->addRow(object_ptr<Ui::VerticalLayout>(box));
	const auto stop = [=] {
		++*generation;
		request->cancel();
		modelRows->clear();
	};
	const auto status = [=](ServiceResult response) {
		result->setText(ServiceErrorText(response.error, response.status));
	};
	if (IsLanguageModelProtocol(service.protocol)) {
		box->addButton(tr::lng_serein_service_models(), [=] {
			stop();
			result->setText(tr::lng_serein_service_testing(tr::now));
			request->models(service, crl::guard(box, [=](ServiceResult response) {
				if (response.error != ServiceError::None) {
					status(std::move(response));
					return;
				}
				const auto object = QJsonDocument::fromJson(response.body).object();
				const auto list = object.value(u"data"_q).toArray();
				auto models = QStringList();
				auto valid = object.value(u"data"_q).isArray()
					&& !list.isEmpty() && list.size() <= 2000;
				for (const auto &entry : list) {
					const auto value = entry.toObject().value(u"id"_q);
					const auto id = value.toString();
					valid = valid && value.isString() && !id.isEmpty()
						&& id.size() <= 256 && !id.contains('\n')
						&& !id.contains('\r') && !id.contains(QChar(0))
						&& QString::fromUtf8(id.toUtf8()) == id;
					models.push_back(id);
				}
				if (!valid) {
					status({ .error = ServiceError::Response });
					return;
				}
				models.removeDuplicates();
				models.sort();
				result->setText(tr::lng_serein_service_models_about(tr::now));
				for (const auto &id : models) {
					const auto row = modelRows->add(object_ptr<Ui::SettingsButton>(
						modelRows, rpl::single(id), st::settingsButtonNoIcon));
					row->setClickedCallback([=] { chooseModel(id); box->closeBox(); });
				}
			}));
		});
	}
	if (service.kind == ServiceKind::Translation) {
		box->addButton(tr::lng_serein_service_test_translation(), [=] {
			stop();
			result->setText(tr::lng_serein_service_testing(tr::now));
			const auto call = BuildTranslationCall(
				service,
				{ u"Hello, world!"_q },
				u"zh"_q);
			request->translate(service, call, crl::guard(box, [=](
					ServiceResult response) {
				if (response.error != ServiceError::None) {
					status(std::move(response));
					return;
				}
				const auto values = ParseTranslationResponse(
					service,
					response.body,
					1);
				if (!values || values->front().size() > 16384) {
					status({ .error = ServiceError::Response });
				} else {
					result->setText(values->front());
				}
			}));
		});
	} else {
		box->addButton(tr::lng_serein_service_test_audio(), [=] {
			stop();
			const auto expected = *generation;
			FileDialog::GetOpenPath(Core::App().getFileDialogParent(),
				tr::lng_serein_service_test_audio(tr::now),
				tr::lng_serein_service_audio_filter(tr::now),
				crl::guard(box, [=](FileDialog::OpenResult &&selected) {
					if (expected != *generation || selected.paths.isEmpty()) {
						return;
					}
					auto file = QFile(selected.paths.front());
					if (!file.open(QIODevice::ReadOnly)) {
						result->setText(tr::lng_serein_service_audio_read_error(tr::now));
						return;
					}
					const auto bytes = file.read(24 * 1024 * 1024 + 1);
					const auto name = QFileInfo(file).fileName();
					file.close();
					box->uiShow()->showBox(Ui::MakeConfirmBox({
						.text = tr::lng_serein_service_audio_upload(tr::now)
							+ u"\n\n"_q + name + u"\n"_q + ServiceEndpoint(service).toDisplayString(),
						.confirmed = crl::guard(box, [=](Fn<void()> close) {
							close();
							if (expected != *generation) {
								return;
							}
							result->setText(tr::lng_serein_service_testing(tr::now));
							request->audio(service, bytes, name,
								crl::guard(box, [=](ServiceResult response) {
									if (response.error != ServiceError::None) {
										status(std::move(response));
										return;
									}
									const auto text = QJsonDocument::fromJson(response.body).object().value(u"text"_q).toString();
									if (text.isEmpty() || text.size() > 16384
										|| text.contains(QChar(0))
										|| QString::fromUtf8(text.toUtf8()) != text) {
										status({ .error = ServiceError::Response });
									} else {
										result->setText(text);
									}
								}));
						}),
					}));
				}));
		});
	}
	box->addButton(tr::lng_cancel(), [=] {
		stop();
		box->closeBox();
	});
}

void ServiceBox(
		not_null<Ui::GenericBox*> box,
		ServicesConfig current,
		ServiceDefinition original,
		Fn<void(ServicesConfig)> saved) {
	box->setTitle(tr::lng_serein_service_edit());

	const auto add = [&](rpl::producer<QString> title,
			const QString &value, bool multiline = false) {
		box->addRow(object_ptr<Ui::FlatLabel>(box, std::move(title), st::boxLabel));
		return box->addRow(object_ptr<Ui::InputField>(
			box,
			st::defaultInputField,
			multiline ? Ui::InputField::Mode::MultiLine : Ui::InputField::Mode::SingleLine,
			rpl::single(QString()),
			value));
	};
	const auto name = add(tr::lng_serein_service_name(), original.name);
	const auto url = add(tr::lng_serein_service_url(), original.baseUrl.toString());
	const auto endpoint = add(tr::lng_serein_service_endpoint(), original.endpoint);
	const auto openai = IsLanguageModelProtocol(original.protocol);
	const auto translation = original.kind == ServiceKind::Translation;
	const auto model = openai ? add(tr::lng_serein_service_model(), original.model) : nullptr;
	const auto system = openai && translation
		? add(tr::lng_serein_service_system_prompt(), original.systemPrompt, true) : nullptr;
	const auto prompt = openai ? add(tr::lng_serein_service_prompt(), original.prompt, true) : nullptr;
	const auto language = !translation ? add(tr::lng_serein_service_language(), original.language) : nullptr;
	const auto region = (original.protocol == u"azure"_q)
		? add(tr::lng_serein_service_region(), original.region)
		: nullptr;
	const auto temperature = openai ? add(tr::lng_serein_service_temperature(),
		original.temperature ? QString::number(*original.temperature) : QString()) : nullptr;
	const auto keyed = !IsKeylessProtocol(original.protocol);
	const auto enabled = box->lifetime().make_state<bool>(
		keyed && original.useKey);
	auto key = (Ui::PasswordInput*)nullptr;
	if (keyed) {
		const auto useKey = box->addRow(object_ptr<Ui::SettingsButton>(
			box, tr::lng_serein_service_use_key(), st::settingsButtonNoIcon));
		useKey->toggleOn(rpl::single(original.useKey));
		useKey->toggledChanges() | rpl::on_next([=](bool value) {
			*enabled = value;
		}, box->lifetime());
		const auto keyRow = box->addRow(object_ptr<Ui::RpWidget>(box));
		keyRow->resize(keyRow->width(), st::defaultInputField.heightMin);
		key = Ui::CreateChild<Ui::PasswordInput>(
			keyRow, st::defaultInputField, tr::lng_serein_service_key());
		keyRow->widthValue() | rpl::on_next([=](int width) {
			key->resize(width, key->height());
		}, keyRow->lifetime());
	}
	box->addRow(object_ptr<Ui::FlatLabel>(box, tr::lng_serein_service_edit_about(), st::boxLabel));
	box->addButton(tr::lng_settings_save(), [=] {
		if (!ServicesUnchanged(box, current)) {
			return;
		}
		auto service = original;
		service.name = name->getLastText().trimmed();
		service.baseUrl = QUrl(url->getLastText().trimmed(), QUrl::StrictMode);
		service.endpoint = endpoint->getLastText().trimmed();
		service.model = model ? model->getLastText().trimmed() : QString();
		service.systemPrompt = system ? system->getLastText() : QString();
		service.prompt = prompt ? prompt->getLastText() : QString();
		service.language = language ? language->getLastText().trimmed() : QString();
		service.region = region
			? region->getLastText().trimmed().toLower()
			: QString();
		service.useKey = *enabled;
		service.temperature = std::nullopt;
		if (temperature && !temperature->getLastText().trimmed().isEmpty()) {
			auto ok = false;
			service.temperature = temperature->getLastText().trimmed().toDouble(&ok);
			if (!ok) {
				temperature->showError();
				return;
			}
		}
		const auto value = ServiceToInstance(service);
		if (!ParseService(value)) {
			box->showToast(tr::lng_serein_service_invalid(tr::now));
			return;
		}
		auto updated = current;
		const auto i = ranges::find(
			updated.instances,
			service.id,
			&ServiceInstance::id);
		const auto found = (i != updated.instances.end());
		if (found) {
			*i = value;
		} else {
			updated.instances.push_back(value);
		}
		auto secret = key ? key->getLastText().toUtf8() : QByteArray();
		if (service.useKey) {
			const auto account = CredentialAccount(service);
			const auto error = !secret.isEmpty()
				? WriteCredential(account, secret)
				: ReadCredential(account).error;
			if (error != CredentialError::None) {
				secret.fill('\0');
				key->showError();
				box->showToast(tr::lng_serein_service_key_error(tr::now));
				return;
			}
		}
		secret.fill('\0');
		if (!SetServices(updated)) {
			box->showToast(tr::lng_serein_service_invalid(tr::now));
			return;
		}
		Core::App().saveSettingsDelayed();
		const auto oldAccount = CredentialAccount(original);
		if (found && oldAccount != CredentialAccount(service)
			&& !CredentialUsed(updated, oldAccount)
			&& RemoveServiceCredential(original) != CredentialError::None) {
			box->uiShow()->showToast(tr::lng_serein_service_key_cleanup(tr::now));
		}
		saved(updated);
		box->closeBox();
	});
	if (FindService(current, original.id)) {
		box->addButton(tr::lng_serein_service_test(), [=] {
			if (!ServicesUnchanged(box, current)) {
				return;
			}
			box->uiShow()->showBox(Box(ServiceTestBox, original,
				crl::guard(box, [=](QString id) {
					if (model) {
						model->setTextWithTags({ std::move(id) });
					}
				})));
		});
		box->addButton(tr::lng_box_delete(), [=] {
			box->uiShow()->showBox(Ui::MakeConfirmBox({
				.text = tr::lng_serein_service_delete_confirm(tr::now),
				.confirmed = crl::guard(box, [=](Fn<void()> close) {
					if (!ServicesUnchanged(box, current)) {
						return;
					}
					auto updated = current;
					updated.instances.erase(
						ranges::remove(
							updated.instances,
							original.id,
							&ServiceInstance::id),
						end(updated.instances));
					for (const auto selected : {
							&updated.translation,
							&updated.transcription }) {
						if (*selected == original.id) {
							selected->clear();
						}
					}
					const auto account = CredentialAccount(original);
					if (!SetServices(updated)) {
						box->showToast(tr::lng_serein_service_invalid(tr::now));
						return;
					}
					Core::App().saveSettingsDelayed();
					if (!CredentialUsed(updated, account)
						&& RemoveServiceCredential(original) != CredentialError::None) {
						box->uiShow()->showToast(
							tr::lng_serein_service_key_cleanup(tr::now));
					}
					saved(updated);
					close();
					box->closeBox();
				}),
				.confirmText = tr::lng_box_delete(),
			}));
		});
	}
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

bool ServicesUnchanged(
		not_null<Ui::GenericBox*> box,
		const ServicesConfig &expected) {
	if (Services() == expected) {
		return true;
	}
	box->showToast(tr::lng_serein_config_changed_error(tr::now));
	return false;
}

void ServicesBox(not_null<Ui::GenericBox*> box, ServicesConfig initial) {
	box->setTitle(tr::lng_serein_services());

	box->addRow(object_ptr<Ui::FlatLabel>(box, tr::lng_serein_services_about(), st::boxLabel));
	const auto state = box->lifetime().make_state<
		rpl::variable<ServicesConfig>>(initial);
	const auto rows = box->addRow(object_ptr<Ui::VerticalLayout>(box));
	const auto changed = crl::guard(box, [=](ServicesConfig value) {
		crl::on_main(box, [=] { *state = value; });
	});
	state->value() | rpl::on_next([=](const ServicesConfig &current) {
		rows->clear();
		const auto add = [&](QString title, Fn<void()> click) {
			const auto row = rows->add(object_ptr<Ui::SettingsButton>(
				rows, rpl::single(std::move(title)), st::settingsButtonNoIcon));
			row->setClickedCallback(std::move(click));
			return row;
		};
		for (const auto &instance : current.instances) {
			const auto service = ParseService(instance);
			if (service) {
				add(service->name, [=] {
					box->uiShow()->showBox(Box(ServiceBox, current, *service, changed));
				});
			}
		}
		struct Preset {
			tr::phrase<> title;
			QString protocol;
			QString baseUrl;
			QString endpoint;
			QString model;
			ServiceKind kind = ServiceKind::Translation;
			bool useKey = true;
		};
		const auto presets = std::array{
			Preset{ tr::lng_serein_service_add_openai, u"openai"_q,
				u"https://api.openai.com/v1/"_q, u"chat/completions"_q },
			Preset{ tr::lng_serein_service_add_gemini, u"openai"_q,
				u"https://generativelanguage.googleapis.com/v1beta/openai/"_q,
				u"chat/completions"_q, u"gemini-2.5-flash"_q },
			Preset{ tr::lng_serein_service_add_anthropic, u"anthropic"_q,
				u"https://api.anthropic.com/v1/"_q, u"messages"_q,
				u"claude-sonnet-4-5"_q },
			Preset{ tr::lng_serein_service_add_deepl, u"deepl"_q,
				u"https://api.deepl.com/v2/"_q, u"translate"_q },
			Preset{ tr::lng_serein_service_add_deeplx, u"deeplx"_q,
				u"http://127.0.0.1:1188/"_q, u"translate"_q, QString(),
				ServiceKind::Translation, false },
			Preset{ tr::lng_serein_service_add_google, u"google"_q,
				u"https://translate.googleapis.com/"_q, u"translate_a/single"_q,
				QString(), ServiceKind::Translation, false },
			Preset{ tr::lng_serein_service_add_yandex, u"yandex"_q,
				u"https://translate.yandex.net/api/v1/tr.json/"_q, u"translate"_q,
				QString(), ServiceKind::Translation, false },
			Preset{ tr::lng_serein_service_add_azure, u"azure"_q,
				u"https://api.cognitive.microsofttranslator.com/"_q,
				u"translate"_q },
			Preset{ tr::lng_serein_service_add_transmart, u"transmart"_q,
				u"https://transmart.qq.com/"_q, u"api/imt"_q,
				QString(), ServiceKind::Translation, false },
			Preset{ tr::lng_serein_service_add_transcription, u"openai"_q,
				u"https://api.openai.com/v1/"_q, u"audio/transcriptions"_q,
				QString(), ServiceKind::Transcription },
		};
		for (const auto &preset : presets) {
			add(preset.title(tr::now), [=] {
				auto service = ServiceDefinition{
					.id = QUuid::createUuid().toString(QUuid::WithoutBraces),
					.kind = preset.kind,
					.protocol = preset.protocol,
					.baseUrl = QUrl(preset.baseUrl),
					.endpoint = preset.endpoint,
					.model = preset.model,
					.credentialRef = QUuid::createUuid().toString(QUuid::WithoutBraces),
					.useKey = preset.useKey,
				};
				box->uiShow()->showBox(Box(ServiceBox, current, std::move(service), changed));
			});
		}
	}, box->lifetime());
	box->addButton(tr::lng_box_ok(), [=] { box->closeBox(); });
}

} // namespace Serein
