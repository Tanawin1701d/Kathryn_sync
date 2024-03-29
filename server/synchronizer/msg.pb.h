// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg.proto

#ifndef PROTOBUF_INCLUDED_msg_2eproto
#define PROTOBUF_INCLUDED_msg_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_msg_2eproto 

namespace protobuf_msg_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_msg_2eproto
class msg;
class msgDefaultTypeInternal;
extern msgDefaultTypeInternal _msg_default_instance_;
namespace google {
namespace protobuf {
template<> ::msg* Arena::CreateMaybeMessage<::msg>(Arena*);
}  // namespace protobuf
}  // namespace google

// ===================================================================

class msg : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:msg) */ {
 public:
  msg();
  virtual ~msg();

  msg(const msg& from);

  inline msg& operator=(const msg& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  msg(msg&& from) noexcept
    : msg() {
    *this = ::std::move(from);
  }

  inline msg& operator=(msg&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const msg& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const msg* internal_default_instance() {
    return reinterpret_cast<const msg*>(
               &_msg_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(msg* other);
  friend void swap(msg& a, msg& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline msg* New() const final {
    return CreateMaybeMessage<msg>(NULL);
  }

  msg* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<msg>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const msg& from);
  void MergeFrom(const msg& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(msg* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string uuid = 1;
  void clear_uuid();
  static const int kUuidFieldNumber = 1;
  const ::std::string& uuid() const;
  void set_uuid(const ::std::string& value);
  #if LANG_CXX11
  void set_uuid(::std::string&& value);
  #endif
  void set_uuid(const char* value);
  void set_uuid(const char* value, size_t size);
  ::std::string* mutable_uuid();
  ::std::string* release_uuid();
  void set_allocated_uuid(::std::string* uuid);

  // string author = 2;
  void clear_author();
  static const int kAuthorFieldNumber = 2;
  const ::std::string& author() const;
  void set_author(const ::std::string& value);
  #if LANG_CXX11
  void set_author(::std::string&& value);
  #endif
  void set_author(const char* value);
  void set_author(const char* value, size_t size);
  ::std::string* mutable_author();
  ::std::string* release_author();
  void set_allocated_author(::std::string* author);

  // string message = 3;
  void clear_message();
  static const int kMessageFieldNumber = 3;
  const ::std::string& message() const;
  void set_message(const ::std::string& value);
  #if LANG_CXX11
  void set_message(::std::string&& value);
  #endif
  void set_message(const char* value);
  void set_message(const char* value, size_t size);
  ::std::string* mutable_message();
  ::std::string* release_message();
  void set_allocated_message(::std::string* message);

  // bytes image = 5;
  void clear_image();
  static const int kImageFieldNumber = 5;
  const ::std::string& image() const;
  void set_image(const ::std::string& value);
  #if LANG_CXX11
  void set_image(::std::string&& value);
  #endif
  void set_image(const char* value);
  void set_image(const void* value, size_t size);
  ::std::string* mutable_image();
  ::std::string* release_image();
  void set_allocated_image(::std::string* image);

  // uint32 likes = 4;
  void clear_likes();
  static const int kLikesFieldNumber = 4;
  ::google::protobuf::uint32 likes() const;
  void set_likes(::google::protobuf::uint32 value);

  // bool needFeed = 6;
  void clear_needfeed();
  static const int kNeedFeedFieldNumber = 6;
  bool needfeed() const;
  void set_needfeed(bool value);

  // bool needImage = 7;
  void clear_needimage();
  static const int kNeedImageFieldNumber = 7;
  bool needimage() const;
  void set_needimage(bool value);

  // bool isDelete = 8;
  void clear_isdelete();
  static const int kIsDeleteFieldNumber = 8;
  bool isdelete() const;
  void set_isdelete(bool value);

  // bool isImageDelete = 9;
  void clear_isimagedelete();
  static const int kIsImageDeleteFieldNumber = 9;
  bool isimagedelete() const;
  void set_isimagedelete(bool value);

  // @@protoc_insertion_point(class_scope:msg)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr uuid_;
  ::google::protobuf::internal::ArenaStringPtr author_;
  ::google::protobuf::internal::ArenaStringPtr message_;
  ::google::protobuf::internal::ArenaStringPtr image_;
  ::google::protobuf::uint32 likes_;
  bool needfeed_;
  bool needimage_;
  bool isdelete_;
  bool isimagedelete_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_msg_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// msg

// string uuid = 1;
inline void msg::clear_uuid() {
  uuid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& msg::uuid() const {
  // @@protoc_insertion_point(field_get:msg.uuid)
  return uuid_.GetNoArena();
}
inline void msg::set_uuid(const ::std::string& value) {
  
  uuid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:msg.uuid)
}
#if LANG_CXX11
inline void msg::set_uuid(::std::string&& value) {
  
  uuid_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:msg.uuid)
}
#endif
inline void msg::set_uuid(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  uuid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:msg.uuid)
}
inline void msg::set_uuid(const char* value, size_t size) {
  
  uuid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:msg.uuid)
}
inline ::std::string* msg::mutable_uuid() {
  
  // @@protoc_insertion_point(field_mutable:msg.uuid)
  return uuid_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* msg::release_uuid() {
  // @@protoc_insertion_point(field_release:msg.uuid)
  
  return uuid_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void msg::set_allocated_uuid(::std::string* uuid) {
  if (uuid != NULL) {
    
  } else {
    
  }
  uuid_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), uuid);
  // @@protoc_insertion_point(field_set_allocated:msg.uuid)
}

// string author = 2;
inline void msg::clear_author() {
  author_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& msg::author() const {
  // @@protoc_insertion_point(field_get:msg.author)
  return author_.GetNoArena();
}
inline void msg::set_author(const ::std::string& value) {
  
  author_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:msg.author)
}
#if LANG_CXX11
inline void msg::set_author(::std::string&& value) {
  
  author_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:msg.author)
}
#endif
inline void msg::set_author(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  author_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:msg.author)
}
inline void msg::set_author(const char* value, size_t size) {
  
  author_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:msg.author)
}
inline ::std::string* msg::mutable_author() {
  
  // @@protoc_insertion_point(field_mutable:msg.author)
  return author_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* msg::release_author() {
  // @@protoc_insertion_point(field_release:msg.author)
  
  return author_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void msg::set_allocated_author(::std::string* author) {
  if (author != NULL) {
    
  } else {
    
  }
  author_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), author);
  // @@protoc_insertion_point(field_set_allocated:msg.author)
}

// string message = 3;
inline void msg::clear_message() {
  message_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& msg::message() const {
  // @@protoc_insertion_point(field_get:msg.message)
  return message_.GetNoArena();
}
inline void msg::set_message(const ::std::string& value) {
  
  message_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:msg.message)
}
#if LANG_CXX11
inline void msg::set_message(::std::string&& value) {
  
  message_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:msg.message)
}
#endif
inline void msg::set_message(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  message_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:msg.message)
}
inline void msg::set_message(const char* value, size_t size) {
  
  message_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:msg.message)
}
inline ::std::string* msg::mutable_message() {
  
  // @@protoc_insertion_point(field_mutable:msg.message)
  return message_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* msg::release_message() {
  // @@protoc_insertion_point(field_release:msg.message)
  
  return message_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void msg::set_allocated_message(::std::string* message) {
  if (message != NULL) {
    
  } else {
    
  }
  message_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), message);
  // @@protoc_insertion_point(field_set_allocated:msg.message)
}

// uint32 likes = 4;
inline void msg::clear_likes() {
  likes_ = 0u;
}
inline ::google::protobuf::uint32 msg::likes() const {
  // @@protoc_insertion_point(field_get:msg.likes)
  return likes_;
}
inline void msg::set_likes(::google::protobuf::uint32 value) {
  
  likes_ = value;
  // @@protoc_insertion_point(field_set:msg.likes)
}

// bytes image = 5;
inline void msg::clear_image() {
  image_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& msg::image() const {
  // @@protoc_insertion_point(field_get:msg.image)
  return image_.GetNoArena();
}
inline void msg::set_image(const ::std::string& value) {
  
  image_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:msg.image)
}
#if LANG_CXX11
inline void msg::set_image(::std::string&& value) {
  
  image_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:msg.image)
}
#endif
inline void msg::set_image(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  image_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:msg.image)
}
inline void msg::set_image(const void* value, size_t size) {
  
  image_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:msg.image)
}
inline ::std::string* msg::mutable_image() {
  
  // @@protoc_insertion_point(field_mutable:msg.image)
  return image_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* msg::release_image() {
  // @@protoc_insertion_point(field_release:msg.image)
  
  return image_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void msg::set_allocated_image(::std::string* image) {
  if (image != NULL) {
    
  } else {
    
  }
  image_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), image);
  // @@protoc_insertion_point(field_set_allocated:msg.image)
}

// bool needFeed = 6;
inline void msg::clear_needfeed() {
  needfeed_ = false;
}
inline bool msg::needfeed() const {
  // @@protoc_insertion_point(field_get:msg.needFeed)
  return needfeed_;
}
inline void msg::set_needfeed(bool value) {
  
  needfeed_ = value;
  // @@protoc_insertion_point(field_set:msg.needFeed)
}

// bool needImage = 7;
inline void msg::clear_needimage() {
  needimage_ = false;
}
inline bool msg::needimage() const {
  // @@protoc_insertion_point(field_get:msg.needImage)
  return needimage_;
}
inline void msg::set_needimage(bool value) {
  
  needimage_ = value;
  // @@protoc_insertion_point(field_set:msg.needImage)
}

// bool isDelete = 8;
inline void msg::clear_isdelete() {
  isdelete_ = false;
}
inline bool msg::isdelete() const {
  // @@protoc_insertion_point(field_get:msg.isDelete)
  return isdelete_;
}
inline void msg::set_isdelete(bool value) {
  
  isdelete_ = value;
  // @@protoc_insertion_point(field_set:msg.isDelete)
}

// bool isImageDelete = 9;
inline void msg::clear_isimagedelete() {
  isimagedelete_ = false;
}
inline bool msg::isimagedelete() const {
  // @@protoc_insertion_point(field_get:msg.isImageDelete)
  return isimagedelete_;
}
inline void msg::set_isimagedelete(bool value) {
  
  isimagedelete_ = value;
  // @@protoc_insertion_point(field_set:msg.isImageDelete)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_msg_2eproto
