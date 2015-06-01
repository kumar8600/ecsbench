#include <vector>

#include <benchmark/benchmark.h>

#include <entityx/entityx.h>

// Components

template <int N>
struct FooComponent
{
    int n = N;
};

// Utilities

template <int N>
void AddComponents(entityx::Entity &entity)
{
    entity.assign<FooComponent<N - 1>>();
    AddComponents<N - 1>(entity);
}

template <>
void AddComponents<0>(entityx::Entity &)
{
}

template <int N>
void RemoveComponents(entityx::Entity &entity)
{
    entity.remove<FooComponent<N - 1>>();
    RemoveComponents<N - 1>(entity);
}

template <>
void RemoveComponents<0>(entityx::Entity &)
{
}

// Benchmarks

static void entityx_CreateEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        entityx::EntityX world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            world.entities.create();
        }
    }
}
BENCHMARK(entityx_CreateEntities)->Range(1<<10, 1<<17);


static void entityx_DestroyEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        std::vector<entityx::Entity> entities(state.range_x());
        entityx::EntityX world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.entities.create();
            e.assign<FooComponent<0>>();
            entities[i] = e;
        }
        for (auto &e : entities)
        {
            e.destroy();
        }
    }
}
BENCHMARK(entityx_DestroyEntities)->Range(1<<10, 1<<17);

template <int N>
static void entityx_AddComponents(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        entityx::EntityX world;
        auto e = world.entities.create();
        AddComponents<N>(e);
    }
}
BENCHMARK_TEMPLATE(entityx_AddComponents, 32);
BENCHMARK_TEMPLATE(entityx_AddComponents, 64);
BENCHMARK_TEMPLATE(entityx_AddComponents, 128);
BENCHMARK_TEMPLATE(entityx_AddComponents, 256);

template <int N>
static void entityx_RemoveComponents(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        entityx::EntityX world;
        auto e = world.entities.create();
        AddComponents<N>(e);
        RemoveComponents<N>(e);
    }
}
BENCHMARK_TEMPLATE(entityx_RemoveComponents, 32);
BENCHMARK_TEMPLATE(entityx_RemoveComponents, 64);
BENCHMARK_TEMPLATE(entityx_RemoveComponents, 128);
BENCHMARK_TEMPLATE(entityx_RemoveComponents, 256);

static void entityx_IterateEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        entityx::EntityX world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.entities.create();
            e.assign<FooComponent<0>>();
            e.assign<FooComponent<1>>();
        }
        entityx::ComponentHandle<FooComponent<0>> c0;
        entityx::ComponentHandle<FooComponent<1>> c1;
        for (auto e : world.entities.entities_with_components(c0, c1))
        {
            c0->n += c1->n;
        }
    }
}
BENCHMARK(entityx_IterateEntities)->Range(1<<10, 1<<17);

static void entityx_GetComponentFromEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        std::vector<entityx::Entity> entities(state.range_x());
        entityx::EntityX world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.entities.create();
            e.assign<FooComponent<0>>();
            entities[i] = e;
        }
        for (auto &e : entities)
        {
            e.component<FooComponent<0>>();
        }
    }
}
BENCHMARK(entityx_GetComponentFromEntities)->Range(1<<10, 1<<17);
