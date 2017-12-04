/*
 * ++C - C++ introduction
 * Copyright (C) 2013, 2014, 2015, 2016 Wilhelm Meier <wilhelm.meier@hs-kl.de>
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

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libwm.h"

#ifdef _MSC_VER
# pragma message("--------------------------------------------------------------------------------------")
# pragma message("For compilation on M$-Windows / M$-VisualStudio please read instructions in " __FILE__)
# pragma message("--------------------------------------------------------------------------------------")
/*
 * In this project (lib):
 * Projekteigenschaften->Konfigurationseigenschaften->C/C++->Allgemein->Zusaetzliche Inkludeverzeichnisse: C:\local\boost_1_55_0
 * Projekteigenschaften->Konfigurationseigenschaften->Linker->System->SubSystem: KONSOLE
 * Projekteigenschaften->Konfigurationseigenschaften->Linker->Allgemein->Zusaetzliche Bibliotheksverzeichnisse: C:\local\boost_1_55_0\lib32-msvc-12.0
 * Systemeinstellungen->System->Umgebungsvariablen->Path (hinzufuegen): C:\local\boost_1_55_0\lib32-msvc-12.0
 *
 * In another project
 * Projekteigenschaften->Konfigurationseigenschaften->Linker->System->SubSystem: KONSOLE
 * Projekteigenschaften->Konfigurationseigenschaften->Linker->Allgemein->Zusaetzliche Bibliotheksverzeichnisse: C:\local\boost_1_55_0\lib32-msvc-12.0;C:\Users\lmeier\Documents\Visual Studio 2013\Projects\lib\Debug
 * Projekteigenschaften->Konfigurationseigenschaften->Linker->Eingabe->Zusaetzlich Abhaengigkeiten: lib.lib
 *
 */
#endif
