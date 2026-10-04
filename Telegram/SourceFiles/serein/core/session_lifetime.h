#pragma once

#include <gsl/pointers>
#include <rpl/filter.h>
#include <rpl/producer.h>
#include <rpl/take.h>

namespace Serein::details {

template <typename Session, typename Cleanup>
void BindSessionCleanup(
		gsl::not_null<Session*> session,
		rpl::producer<Session*> published,
		Cleanup cleanup,
		rpl::lifetime &subscriptionLifetime) {
	std::move(
		published
	) | rpl::filter([=](Session *value) {
		return (value == session.get());
	}) | rpl::take(1) | rpl::on_next([=] {
		session->lifetime().add(cleanup);
	}, subscriptionLifetime);
}

} // namespace Serein::details
