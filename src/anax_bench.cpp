#include <vector>

#include <benchmark/benchmark.h>

#include <anax/anax.hpp>

// Components

template <int N>
struct FooComponent
        : public anax::Component<FooComponent<N>>
{
    int n = N;
};

// Systems

struct BarSystem
        : public anax::System<BarSystem>
{
    BarSystem()
        : Base(anax::ComponentFilter().requires<FooComponent<0>, FooComponent<1>>())
    {
    }

    void update()
    {
        auto entities = getEntities();

        for(auto e : entities)
        {
            auto c0 = e.getComponent<FooComponent<0>>();
            auto c1 = e.getComponent<FooComponent<1>>();

            c0.n += c1.n;
        }
    }
};

// Utilities

template <int N>
void AddComponents(anax::Entity &entity)
{
    entity.addComponent<FooComponent<N - 1>>();
    AddComponents<N - 1>(entity);
}

template <>
void AddComponents<0>(anax::Entity &)
{
}

template <int N>
void RemoveComponents(anax::Entity &entity)
{
    entity.removeComponent<FooComponent<N - 1>>();
    RemoveComponents<N - 1>(entity);
}

template <>
void RemoveComponents<0>(anax::Entity &)
{
}

// Benchmarks

static void anax_CreateEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        anax::World world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.createEntity();
            e.activate();
        }
    }
}
BENCHMARK(anax_CreateEntities)->Range(1<<10, 1<<17);


static void anax_DestroyEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        std::vector<anax::Entity> entities(state.range_x());
        anax::World world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.createEntity();
            e.addComponent<FooComponent<0>>();
            e.activate();
            entities[i] = e;
        }
        for (auto &e : entities)
        {
            e.kill();
        }
    }
}
BENCHMARK(anax_DestroyEntities)->Range(1<<10, 1<<17);

template <int N>
static void anax_AddComponents(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        anax::World world;
        auto e = world.createEntity();
        AddComponents<N>(e);
        e.activate();
    }
}
BENCHMARK_TEMPLATE(anax_AddComponents, 32);
BENCHMARK_TEMPLATE(anax_AddComponents, 64);
BENCHMARK_TEMPLATE(anax_AddComponents, 128);
BENCHMARK_TEMPLATE(anax_AddComponents, 256);

template <int N>
static void anax_RemoveComponents(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        anax::World world;
        auto e = world.createEntity();
        AddComponents<N>(e);
        e.activate();
        RemoveComponents<N>(e);
    }
}
BENCHMARK_TEMPLATE(anax_RemoveComponents, 32);
BENCHMARK_TEMPLATE(anax_RemoveComponents, 64);
BENCHMARK_TEMPLATE(anax_RemoveComponents, 128);
BENCHMARK_TEMPLATE(anax_RemoveComponents, 256);

static void anax_IterateEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        anax::World world;
        BarSystem system;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.createEntity();
            e.addComponent<FooComponent<0>>();
            e.addComponent<FooComponent<1>>();
            e.activate();
        }
        world.addSystem(system);
        world.refresh();
    }
}
BENCHMARK(anax_IterateEntities)->Range(1<<10, 1<<17);

static void anax_GetComponentFromEntities(benchmark::State &state)
{
    while (state.KeepRunning())
    {
        std::vector<anax::Entity> entities(state.range_x());
        anax::World world;
        for (int i = 0; i < state.range_x(); ++i)
        {
            auto e = world.createEntity();
            e.addComponent<FooComponent<0>>();
            entities[i] = e;
            e.activate();
        }
        for (auto &e : entities)
        {
            e.getComponent<FooComponent<0>>();
        }
    }
}
BENCHMARK(anax_GetComponentFromEntities)->Range(1<<10, 1<<17);
