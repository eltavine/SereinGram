#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <QtCore/QCoreApplication>

#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
	try {
		auto context = doctest::Context(argc, argv);
#ifdef SEREIN_HAVE_QT_SQL
		auto qtArgc = 1;
		char name[] = "test_serein";
		char *qtArgv[] = { name, nullptr };
		const auto application = QCoreApplication(qtArgc, qtArgv);
#endif // SEREIN_HAVE_QT_SQL
		return context.run();
	} catch (const std::exception &error) {
		std::cerr << "test_serein: " << error.what() << std::endl;
	} catch (...) {
		std::cerr << "test_serein: unknown exception" << std::endl;
	}
	return 1;
}
