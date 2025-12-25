/* Main.cc: Entry point for Silikego CLI
 * Copyright 2012-2025 Vincent Damewood
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <cstring>

#if defined USE_UNIX
#	include <unistd.h>
#	define ISATTY() (isatty(fileno(stdin)) && isatty(fileno(stdout)))
#else
#	define ISATTY() (-1)
#endif

#include <iostream>

#if HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
extern "C" char *readline(const char *);
extern "C" void add_history(char *);
#endif /* HAVE_READLINE */

#include <SilikegoCore/FunctionCaller.h>
#include <SilikegoCore/InfixParser.h>

#include <SilikegoCore/StringSource.h>

int main(int argc, char *argv[])
{
	const char *prompt;
	const char *response;

	if (ISATTY())
	{
		prompt = "> ";
		response = "= ";
	}
	else
	{
		prompt = "";
		response = "";
	}

	Silikego::FunctionCaller caller;
	Silikego::InstallOperators(caller);
	Silikego::InstallFunctions(caller);

	char *expression = NULL;
	char *old_expression = NULL;
	while(-1)
	{
		expression = readline(prompt);

		if(!expression)
		{
			std::free(static_cast<void*>(old_expression));
			old_expression = NULL;
			break;
		}

		if(*expression && (!old_expression || std::strcmp(expression, old_expression) != 0))
			add_history(expression);

		std::free(static_cast<void*>(old_expression));
		old_expression = expression;

		Silikego::SyntaxTreeNode Tree
			= Silikego::ParseInfix(std::unique_ptr<Silikego::DataSource>(new Silikego::StringSource(expression)));
		Silikego::Value result = Tree.evaluate(caller);

		switch (result.status())
		{
		case Silikego::ValueStatus::Integer:
			std::cout << result.toInteger() << std::endl;
			break;
		case Silikego::ValueStatus::Real:
			std::cout << result.toReal() << std::endl;
			break;
		case Silikego::ValueStatus::Error:
			switch(result.toError())
			{
			case Silikego::Error::Memory:
				std::cout << "Error: Out of memory\n";
				break;
			case Silikego::Error::Syntax:
				std::cout << "Error: Syntax error\n";
				break;
			case Silikego::Error::ZeroDivision:
				std::cout << "Error: Division by zero\n";
				break;
			case Silikego::Error::FunctionName:
				std::cout << "Error: Function not found\n";
				break;
			case Silikego::Error::FunctionArguments:
				std::cout << "Error: Bad argument count\n";
				break;
			case Silikego::Error::Domain:
				std::cout << "Error: Domain error\n";
				break;
			case Silikego::Error::Range:
				std::cout << "Error: Range error\n";
				break;
			default:
				std::cout << "Unnexpected error\n";
				break;
			}
		}
	}

	if (ISATTY())
		std::cout << std::endl;
	return 0;
}
