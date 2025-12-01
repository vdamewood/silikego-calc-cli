# FindReadline.cmake: Find Readline
# Copyright 2020, 2025 Vincent Damewood
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

message(CHECK_START "Finding Readline")

if(TARGET Readline::Readline)
	message(CHECK_PASS "Found existing CMake target, skipping.")
	set(PARENT_SCOPE Readline_FOUND 1)
	return()
endif()

find_package(PkgConfig)

if(PkgConfig_FOUND)
	pkg_check_modules(Readline readline IMPORTED_TARGET)
	if(TARGET PkgConfig::Readline AND NOT TARGET Readline::Readline)
		add_library(Readline::Readline ALIAS PkgConfig::Readline)
		message(CHECK_PASS "Found readline.pc with pkg-config")
		set(PARENT_SCOPE Readline_FOUND 1)
		return()
	endif()
endif()

find_library(Readline_LIBRARY readline)
find_path(Readline_INCLUDE_DIR NAMES readline/readline.h)
if(Readline_LIBRARY AND Readline_INCLUDE_DIR)
	add_library(Readline::Readline UNKNOWN IMPORTED GLOBAL)
	set_property(TARGET Readline::Readline PROPERTY IMPORTED_LOCATION "${Readline_LIBRARY}")
	target_include_directories(Readline::Readline INTERFACE "${Readline_INCLUDE_DIR}")
	message(CHECK_PASS "Found ${Readline_LIBRARY} and ${Readline_INCLUDE_DIR}/readline/readline.h")
	set(PARENT_SCOPE Readline_FOUND 1)
	return()
endif()

message(CHECK_FAIL "No suitable library found")
set(PARENT_SCOPE Readline_FOUND 0)
