#pragma once

#include "stores.pb-c.h"

// Protoc generates versions of these for each message type, so we define generic
// versions to simplify passing messages around
typedef size_t (*GetPackedSizeFunc)(const void *msg);
typedef size_t (*PackFunc)(const void *msg, uint8_t *out);
typedef void *(*UnpackFunc)(ProtobufCAllocator *allocator, size_t len, const uint8_t *data);
typedef void (*FreeUnpackedFunc)(void *msg, ProtobufCAllocator *allocator);
// Each module needs to define its own UpdateStoreFunc, so we pass a function pointer
// around to avoid adding functions to headers
typedef void (*UpdateStoreFunc)(ProtobufCBinaryData msg, ProtobufCBinaryData mask);

// Each module registers a StoreFuncs set for the store library to work with
typedef struct StoreFuncs {
  GetPackedSizeFunc get_packed_size;
  PackFunc pack;
  UnpackFunc unpack;
  FreeUnpackedFunc free_unpacked;
  UpdateStoreFunc update_store;
} StoreFuncs;

// Initialize the store. The first call registers funcs and sets up
// communication with parent; subsequent calls only register funcs.
// The intention is to call this from every 'driver'.
void store_init(MxStoreType type, StoreFuncs funcs);

// Drivers allocate their own stores, then register them here. The
// key is a pointer to the associated 'storage' struct, since some
// drivers may be used with multiple storages. If the storage is
// unique, key should be NULL.
void store_register(MxStoreType type, void *store, void *key);

// Gets the matching pointer for the type.
// If key is NULL, it's assumed the store is unique.
void *store_get(MxStoreType type, void *key);

// Call at every store update to export store to parent.
void store_export(MxStoreType type, void *store, void *key);