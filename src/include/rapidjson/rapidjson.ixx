// Tencent is pleased to support the open source community by making RapidJSON available.
//
// Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip.
//
// Licensed under the MIT License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

module;

#include "rapidjson.h"

#include "allocators.h"
#include "cursorstreamwrapper.h"
#include "document.h"
#include "encodedstream.h"
#include "encodings.h"
#include "filereadstream.h"
#include "filewritestream.h"
#include "fwd.h"
#include "istreamwrapper.h"
#include "memorybuffer.h"
#include "memorystream.h"
#include "ostreamwrapper.h"
#include "pointer.h"
#include "prettywriter.h"
#include "reader.h"
#include "schema.h"
#include "stream.h"
#include "stringbuffer.h"
#include "uri.h"
#include "writer.h"

#include "error/en.h"
#include "error/error.h"

#include "internal/biginteger.h"
#include "internal/clzll.h"
#include "internal/diyfp.h"
#include "internal/dtoa.h"
#include "internal/ieee754.h"
#include "internal/itoa.h"
#include "internal/meta.h"
#include "internal/pow10.h"
#include "internal/regex.h"
#include "internal/stack.h"
#include "internal/strfunc.h"
#include "internal/strtod.h"
#include "internal/swap.h"

#ifdef _MSC_VER
#include "msinttypes/inttypes.h"
#include "msinttypes/stdint.h"
#endif

export module rapidjson;

#pragma warning(push)
#pragma warning(disable: 5244)


namespace rapidjson
{
    export using rapidjson::ASCII;
    export using rapidjson::AutoUTF;
    export using rapidjson::AutoUTFInputStream;
    export using rapidjson::AutoUTFOutputStream;
    export using rapidjson::BaseReaderHandler;
    export using rapidjson::BasicIStreamWrapper;
    export using rapidjson::BasicOStreamWrapper;
    export using rapidjson::CreateValueByPointer;
    // export using rapidjson::CrtAllocator;
    export using rapidjson::CursorStreamWrapper;
    export using rapidjson::Document;
    export using rapidjson::EncodedInputStream;
    export using rapidjson::EncodedOutputStream;
    export using rapidjson::EraseValueByPointer;
    export using rapidjson::FileReadStream;
    export using rapidjson::FileWriteStream;
    export using rapidjson::Free;
    // export using rapidjson::GenericArray;
    // export using rapidjson::GenericDocument;
    // export using rapidjson::GenericInsituStringStream;
    // export using rapidjson::GenericMember;
    // export using rapidjson::GenericMemberIterator;
    // export using rapidjson::GenericMemoryBuffer;
    // export using rapidjson::GenericObject;
    // export using rapidjson::GenericPointer;
    // export using rapidjson::GenericReader;
    // export using rapidjson::GenericSchemaDocument;
    // export using rapidjson::GenericSchemaValidator;
    // export using rapidjson::GenericStreamWrapper;
    // export using rapidjson::GenericStringBuffer;
    // export using rapidjson::GenericUri;
    // export using rapidjson::GenericValue;
    export using rapidjson::GetParseError_En;
    export using rapidjson::GetParseErrorFunc;
    export using rapidjson::GetPointerParseError_En;
    export using rapidjson::GetPointerParseErrorFunc;
    export using rapidjson::GetSchemaError_En;
    export using rapidjson::GetSchemaErrorFunc;
    export using rapidjson::GetValidateError_En;
    export using rapidjson::GetValidateErrorFunc;
    export using rapidjson::GetValueByPointer;
    export using rapidjson::GetValueByPointerWithDefault;
    export using rapidjson::IGenericRemoteSchemaDocumentProvider;
    export using rapidjson::InsituStringStream;
    export using rapidjson::IRemoteSchemaDocumentProvider;
    // export using rapidjson::IStreamWrapper;
    export using rapidjson::Malloc;
    export using rapidjson::MemoryBuffer;
    // export using rapidjson::MemoryPoolAllocator;
    export using rapidjson::MemoryStream;
    export using rapidjson::OStreamWrapper;
    export using rapidjson::ParseResult;
    export using rapidjson::Pointer;
    export using rapidjson::PrettyWriter;
    export using rapidjson::PutN;
    export using rapidjson::PutReserve;
    export using rapidjson::PutUnsafe;
    export using rapidjson::Reader;
    export using rapidjson::Realloc;
    export using rapidjson::SchemaDocument;
    export using rapidjson::SchemaValidatingReader;
    export using rapidjson::SchemaValidator;
    export using rapidjson::SetValueByPointer;
    export using rapidjson::size_t;
    export using rapidjson::SizeType;
    export using rapidjson::SkipWhitespace;
    export using rapidjson::Specification;
    export using rapidjson::StdAllocator;
    export using rapidjson::StreamTraits;
    export using rapidjson::StringBuffer;
    export using rapidjson::StringRef;
    export using rapidjson::StringStream;
    export using rapidjson::SwapValueByPointer;
    export using rapidjson::Transcoder;
    export using rapidjson::Type;
    export using rapidjson::Uri;
    export using rapidjson::UTF16;
    export using rapidjson::UTF16BE;
    export using rapidjson::UTF16LE;
    export using rapidjson::UTF32;
    export using rapidjson::UTF32BE;
    export using rapidjson::UTF32LE;
    export using rapidjson::UTF8;
    export using rapidjson::Value;
    export using rapidjson::WIStreamWrapper;
    export using rapidjson::WOStreamWrapper;
    export using rapidjson::Writer;
}

// export using rapidjson::internal::AddConst;
// export using rapidjson::internal::

#pragma warning(pop)