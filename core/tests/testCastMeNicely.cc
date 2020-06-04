#include <cc/stdio>

using namespace cc;

class Entity
{
public:
    String name() const { return name_; }

protected:
    Entity(const String &name):
        name_{name}
    {}

private:
    String name_;
};

class Man
{
public:
    class Instance: public Entity
    {
    private:
        friend class Man;
        Instance(const String &name):
            Entity{name}
        {}
    };

    Man(const String &name): instance_{name} {}

    operator Entity *() { return &instance_; }
    operator const Entity *() const { return &instance_; }

    const Instance *operator->() const { return &instance_; }

private:
    Instance instance_;
};

void writeName(const Entity *entity)
{
    fout() << entity->name() << nl;
}

int main(int argc, char **argv)
{
    writeName(Man{"Richard"});
    fout() << Man{"Rudolf"}->name() << nl;
    return 0;
}
