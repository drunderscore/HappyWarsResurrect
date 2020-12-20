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

#include "BFBR.h"
#include <LibFruit/Data/Stream.h>
#include "Blowfish/blowfish.h"
#include <zlib/zlib.h>
#include <cmath>

namespace LibHappyWars
{
    std::optional<BFBR> BFBR::parse(std::istream& std_stream, const char* key)
    {
        LibFruit::Stream stream(std_stream);
        auto magic = stream.read<u32>();

        if(magic != BFBR::MAGIC) // "BFBR"
        {
            Logger::error().println("BFBR file has incorrect magic value {:#x}", magic);
            return {};
        }

        auto data_size = stream.size() - 4;

        // Blowfish requires proper 8-byte padding when encoding/decoding.
        // BFBR files should always come with this already accounted for.
        if(data_size % 8 != 0)
        {
            Logger::error().println("BFBR file is not properly padded? ({} extraneous bytes)", 8 - (stream.size() % 8));
            return {};
        }

        std::vector<u8> encrypted_data(data_size);
        stream.read(encrypted_data.data(), data_size);

        CBlowFish bf;
        bf.Initialize(reinterpret_cast<unsigned char*>(const_cast<char*>(key)), strlen(key));

        BFBR bfbr;
        bfbr.m_data.resize(encrypted_data.size());
        bf.Decode(encrypted_data.data(), bfbr.m_data.data(), encrypted_data.size());

        z_stream decompress_stream;
        decompress_stream.zalloc = Z_NULL;
        decompress_stream.zfree = Z_NULL;
        decompress_stream.opaque = Z_NULL;
        decompress_stream.avail_in = 0;
        decompress_stream.avail_out = Z_NULL;

        auto zlib_ret_val = inflateInit(&decompress_stream);

        if(zlib_ret_val != Z_OK)
        {
            Logger::error().println("ZLib failed to init inflate? {}", zlib_ret_val);
            return {};
        }

        // TODO: Do this in chunks.

        bfbr.m_decompressed_data.resize(bfbr.header()->uncompressed_size);

        auto header_size = sizeof(Header);
        decompress_stream.avail_in = bfbr.m_data.size() - header_size;
        decompress_stream.next_in = bfbr.m_data.data() + header_size;
        decompress_stream.avail_out = bfbr.header()->uncompressed_size;
        decompress_stream.next_out = bfbr.m_decompressed_data.data();
        zlib_ret_val = inflate(&decompress_stream, Z_NO_FLUSH);
        inflateEnd(&decompress_stream);

        if(zlib_ret_val != Z_STREAM_END)
        {
            Logger::error().println("Didn't reach end of stream after inflating all data. (got return {})", zlib_ret_val);
            return {};
        }

        Logger::dbg().println("Inflated BFBR: {} bytes to {} bytes", bfbr.m_data.size(), bfbr.m_decompressed_data.size());

        return bfbr;
    }
}
