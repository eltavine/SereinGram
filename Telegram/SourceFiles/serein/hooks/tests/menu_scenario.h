#pragma once

#ifdef _DEBUG

#include <gsl/pointers>

namespace Test {
class Runner;
} // namespace Test

namespace Serein::Tests {

void AppendMenuScenario(gsl::not_null<Test::Runner*> runner);

} // namespace Serein::Tests

#endif // _DEBUG
