/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Bouncer.hpp"

namespace pona
{

Bouncer::Bouncer(Ref<MonitorMedia> media)
	: media_(media),
	  mutex_(new Mutex)
{
	media->setMonitor(this);
}

void Bouncer::beforeGrowing(int i, int n)
{
	mutex_->acquire();
	if (media_->size() != intMax)
		while (media_->fill() + n > media_->size())
			itemsRemoved()->wait(mutex_);
}

void Bouncer::afterGrowing(int i, int n)
{
	mutex_->release();
	if (media_->size() != intMax)
		itemsAdded()->wakeupAll();
}

void Bouncer::beforeShrinking(int i, int n)
{
	mutex_->acquire();
	if (media_->size() != intMax)
		while (media_->fill() - n < 0)
			itemsAdded()->wait(mutex_);
}

void Bouncer::afterShrinking(int i, int n)
{
	mutex_->release();
	if (media_->size() != intMax)
		itemsRemoved()->wakeupAll();
}

void Bouncer::beforeAccess(int i, int n)
{
	mutex_->acquire();
}

void Bouncer::afterAccess(int i, int n)
{
	mutex_->release();
}

void Bouncer::beforeReadonlyAccess(int i, int n)
{
	mutex_->acquire();
}

void Bouncer::afterReadonlyAccess(int i, int n)
{
	mutex_->release();
}

Ref<Condition> Bouncer::itemsAdded()
{
	if (!itemsAdded_)
		itemsAdded_ = new Condition();
	return itemsAdded_;
}

Ref<Condition> Bouncer::itemsRemoved()
{
	if (!itemsRemoved_)
		itemsRemoved_ = new Condition();
	return itemsRemoved_;
}

} // namespace pona
