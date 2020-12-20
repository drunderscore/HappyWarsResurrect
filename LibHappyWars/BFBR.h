// Copyright James Puleo 2020
// Copyright LibHappyWars 2020

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <optional>
#include <fstream>
#include <LibFruit/Fruit.h>
#include <vector>

namespace LibHappyWars
{
    // The BFBR file is first compressed with zlib, and then encrypted with
    // Blowfish. The key may(?) depend on the version, but for our use, it
    // probably won't change.
    class BFBR
    {
    public:
        struct Header
        {
            u32 unk_0;
            u32 unk_1;
            u32 unk_2;
            u32 unk_3;
            u32 unk_4;
            u32 unk_5;
            u32 unk_6;
            u32 unk_7;
            u32 unk_8;
            u32 unk_9;
            u32 uncompressed_size;
        };

        static_assert(sizeof(Header) == 0x2C, "BFBR header is not the correct size?");
        static constexpr u32 MAGIC = 0x52424642;
        // This is the key as for game version Ver_1_63_0
        static constexpr const char* DEFAULT_KEY = "04B915BA43FEB5B6";
        static std::optional<BFBR> parse(std::istream&, const char* key = DEFAULT_KEY);

        const Header* header() const { return reinterpret_cast<const Header*>(m_data.data()); }
        const std::vector<u8>& decompressed_data() const { return m_decompressed_data; }
    private:
        BFBR() {}
        std::vector<u8> m_data;
        std::vector<u8> m_decompressed_data;
    };
}
