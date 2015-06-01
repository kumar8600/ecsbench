#include <vector>

#include <benchmark/benchmark.h>

#include <coment/World.h>
#include <coment/Entity.h>
#include <coment/Component.h>
#include <coment/systems/System.h>

// Components

template <int N>
struct FooComponent
  : coment::Component
{
    int n = N;
};

// Utilities

template <int N>
void AddComponents(coment::Entity &entity)
{
    entity.addComponent<FooComponent<N - 1>>();
    AddComponents<N - 1>(entity);
}

template <>
void AddComponents<0>(coment::Entity &)
{
}

template <int N>
void RemoveComponents(coment::Entity &entity)
{
    entity.removeComponent<FooComponent<N - 1>>();
    RemoveComponents<N - 1>(entity);
}

template <>
void RemoveComponents<0>(coment::Entity &)
{
}

// Benchmarks

static void coment_CreateEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        coment::World world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            world.createEntity();
        }
    }
}
BENCHMARK(coment_CreateEntities)->Range(1<<10, 1<<17);


static void coment_DestroyEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        std::vector<coment::Entity> entities(state.range_x());
        coment::World world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.createEntity();
            e.addComponent<FooComponent<0>>();
            entities[i] = e;
        }
        for (auto &e : entities)
        {
            world.destroyEntity(e);
        }
    }
}
BENCHMARK(coment_DestroyEntities)->Range(1<<10, 1<<17);

template <int N>
static void coment_AddComponents(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        coment::World world;
        auto e = world.createEntity();
        AddComponents<N>(e);
    }
}
BENCHMARK_TEMPLATE(coment_AddComponents, 32);
BENCHMARK_TEMPLATE(coment_AddComponents, 64);
BENCHMARK_TEMPLATE(coment_AddComponents, 128);
BENCHMARK_TEMPLATE(coment_AddComponents, 256);

template <int N>
static void coment_RemoveComponents(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        coment::World world;
        auto e = world.createEntity();
        AddComponents<N>(e);
        RemoveComponents<N>(e);
    }
}
BENCHMARK_TEMPLATE(coment_RemoveComponents, 32);
BENCHMARK_TEMPLATE(coment_RemoveComponents, 64);
BENCHMARK_TEMPLATE(coment_RemoveComponents, 128);
BENCHMARK_TEMPLATE(coment_RemoveComponents, 256);

static void coment_IterateEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        coment::World world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.createEntity();
            e.addComponent<FooComponent<0>>();
            e.addComponent<FooComponent<1>>();
        }
        auto fooMap = world.getEntityMap<FooComponent<0>, FooComponent<1>>();
        for (auto &e : fooMap)
        {
            auto c0 = e.getComponent<FooComponent<0>>();
            auto c1 = e.getComponent<FooComponent<1>>();

            c0->n += c1->n;
        }
    }
}
BENCHMARK(coment_IterateEntities)->Range(1<<10, 1<<17);

static void coment_GetComponentFromEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        std::vector<coment::Entity> entities(state.range_x());
        coment::World world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.createEntity();
            e.addComponent<FooComponent<0>>();
            entities[i] = e;
        }
        for (auto &e : entities)
        {
            e.getComponent<FooComponent<0>>();
        }
    }
}
BENCHMARK(coment_GetComponentFromEntities)->Range(1<<10, 1<<17);
