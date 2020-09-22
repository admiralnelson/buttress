#pragma once

typedef unsigned int MeshId;
const MeshId INVALID_MESH = -1;

typedef unsigned int MaterialId;
const MaterialId INVALID_MATERIAL = -1;
typedef unsigned int EntityId;

const EntityId MAX_ENTITIES = 1 * 1'000'000;
const EntityId INVALID_ENTITY = -1;

typedef unsigned int ComponentTypeId;
const ComponentTypeId MAX_COMPONENTS = 32;

typedef std::bitset<MAX_COMPONENTS> ComponentSignature;

typedef unsigned int EventId;
const EventId INVALID_EVENT = -1;
const EventId INVALID_EVENTPARAM = -1;

typedef unsigned int ModelId;
const ModelId INVALID_MODEL = -1;