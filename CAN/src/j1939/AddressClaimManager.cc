/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/AddressClaimManager>
#include <cc/ReadWriteLock>
#include <cc/WriteGuard>
#include <cc/ReadGuard>
#include <cc/Map>

namespace cc::j1939 {

struct AddressClaimManager::State final: public Singleton::State
{
    void establishClaim(uint8_t address, const FunctionId &fid)
    {
        WriteGuard guard{lock_};
        claims_.establish(address, fid);
    }

    bool lookupClaim(uint8_t address, Out<FunctionId> fid) const
    {
        ReadGuard guard{lock_};
        return claims_.lookup(address, &fid);
    }

    bool resolveName(uint8_t address, Out<String> name)
    {
        FunctionId fid;
        {
            ReadGuard guard{lock_};
            if (names_.lookup(address, &name)) {
                return true;
            }
            else if (!claims_.lookup(address, &fid)) {
                return false;
            }
        }
        {
            WriteGuard guard{lock_};
            name = fid.functionName();
            names_.establish(address, *name);
        }
        return true;
    }

    mutable ReadWriteLock lock_;
    Map<uint8_t, FunctionId> claims_;
    Map<uint8_t, String> names_;
};

AddressClaimManager::AddressClaimManager():
    Singleton{instance<State>()}
{}

void AddressClaimManager::establishClaim(uint8_t address, const FunctionId &fid)
{
    me().establishClaim(address, fid);
}

bool AddressClaimManager::lookupClaim(uint8_t address, Out<FunctionId> fid)
{
    return me().lookupClaim(address, &fid);
}

bool AddressClaimManager::resolveName(uint8_t address, Out<String> name)
{
    return me().resolveName(address, &name);
}

String AddressClaimManager::name(uint8_t address)
{
    String h; resolveName(address, &h); return h;
}

AddressClaimManager::State &AddressClaimManager::me()
{
    return Object::me.as<State>();
}

const AddressClaimManager::State &AddressClaimManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::j1939
