/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef COLLECTIONMODELPRIVATE_H
#define COLLECTIONMODELPRIVATE_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QHash>

class CollectionInterface;
class CollectionModel;
class CollectionExtensionInterface;

class CollectionModelPrivate : public QObject
{
   Q_OBJECT
public:
   CollectionModelPrivate(CollectionModel* parent);

   /*
    * This is not very efficient, it doesn't really have to be given the low
    * volume. If it ever have to scale, a better mapToSource using persistent
    * index have to be implemented.
    */
   struct ProxyItem {
      ProxyItem() : parent(nullptr),col(1),row(0),collection(nullptr){}
      int row;
      int col;
      CollectionInterface* collection;
      ProxyItem* parent;
      QVector<ProxyItem*> m_Children;
   };

   QHash<CollectionInterface*,ProxyItem*> m_hBackendsNodes;
   QVector<ProxyItem*>                    m_lTopLevelBackends;
   QVector<CollectionExtensionInterface*> m_lExtensions;
   static CollectionModel*                m_spInstance;

   //Helper
   void registerNew(CollectionInterface* col);

private:
   CollectionModel* q_ptr;

private Q_SLOTS:
   void slotUpdate();
   void slotExtensionDataChanged(const QModelIndex& idx);
};

#endif