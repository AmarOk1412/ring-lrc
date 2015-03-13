/************************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                                  *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "fallbackpersoncollection.h"

//Qt
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>

//Ring
#include "person.h"
#include "personmodel.h"
#include "vcardutils.h"
#include "contactmethod.h"
#include "collectioneditor.h"
#include "delegates/pixmapmanipulationdelegate.h"


class FallbackPersonBackendEditor : public CollectionEditor<Person>
{
public:
   FallbackPersonBackendEditor(CollectionMediator<Person>* m, const QString& path) : CollectionEditor<Person>(m),m_Path(path) {}
   virtual bool save       ( const Person* item ) override;
   virtual bool remove     ( const Person* item ) override;
   virtual bool edit       ( Person*       item ) override;
   virtual bool addNew     ( const Person* item ) override;
   virtual bool addExisting( const Person* item ) override;

   QVector<Person*> m_lItems;
   QString          m_Path  ;

private:
   virtual QVector<Person*> items() const override;
};

class FallbackPersonCollectionPrivate : public QObject
{
   Q_OBJECT
public:
   FallbackPersonCollectionPrivate(FallbackPersonCollection* parent, CollectionMediator<Person>* mediator, const QString& path);
   CollectionMediator<Person>*  m_pMediator;
   QString                      m_Path     ;
   QString                      m_Name     ;

   FallbackPersonCollection* q_ptr;

public Q_SLOTS:
   void loadAsync();
};

FallbackPersonCollectionPrivate::FallbackPersonCollectionPrivate(FallbackPersonCollection* parent, CollectionMediator<Person>* mediator, const QString& path) : q_ptr(parent), m_pMediator(mediator), m_Path(path)
{
   m_Name = path.split('/').last();
   if (m_Name.size())
      m_Name[0] = m_Name[0].toUpper();
   else
      m_Name = "vCard";
}

FallbackPersonCollection::FallbackPersonCollection(CollectionMediator<Person>* mediator, const QString& path, FallbackPersonCollection* parent) :
CollectionInterface(new FallbackPersonBackendEditor(mediator,path),parent),d_ptr(new FallbackPersonCollectionPrivate(this,mediator,path))
{
}

FallbackPersonCollection::~FallbackPersonCollection()
{

}

bool FallbackPersonBackendEditor::save(const Person* item)
{
   QFile file(m_Path+'/'+item->uid()+".vcf");
   file.open(QIODevice::WriteOnly);
   file.write(item->toVCard({}));
   file.close();
   return true;
}

bool FallbackPersonBackendEditor::remove(const Person* item)
{
   Q_UNUSED(item)
   return false;
}

bool FallbackPersonBackendEditor::edit( Person* item)
{
   Q_UNUSED(item)
   return false;
}

bool FallbackPersonBackendEditor::addNew(const Person* item)
{
   Q_UNUSED(item)
   return false;
}

bool FallbackPersonBackendEditor::addExisting(const Person* item)
{
   m_lItems << const_cast<Person*>(item);
   mediator()->addItem(item);
   return true;
}

QVector<Person*> FallbackPersonBackendEditor::items() const
{
   return m_lItems;
}

QString FallbackPersonCollection::name () const
{
   return d_ptr->m_Name;
}

QString FallbackPersonCollection::category () const
{
   return QObject::tr("Contacts");
}

QVariant FallbackPersonCollection::icon() const
{
   return PixmapManipulationDelegate::instance()->collectionIcon(this,PixmapManipulationDelegate::CollectionIconHint::CONTACT);
}

bool FallbackPersonCollection::isEnabled() const
{
   return true;
}

bool FallbackPersonCollection::load()
{
   bool ok;
   QList< Person* > ret =  VCardUtils::loadDir(QUrl(d_ptr->m_Path),ok);
   for(Person* p : ret) {
      editor<Person>()->addExisting(p);
   }

   //Add all sub directories as new backends
   QTimer::singleShot(0,d_ptr,SLOT(loadAsync()));

   return true;
}

bool FallbackPersonCollection::reload()
{
   return false;
}

CollectionInterface::SupportedFeatures FallbackPersonCollection::supportedFeatures() const
{
   return (CollectionInterface::SupportedFeatures)       (
      CollectionInterface::SupportedFeatures::NONE       |
      CollectionInterface::SupportedFeatures::LOAD       |
      CollectionInterface::SupportedFeatures::CLEAR      |
      CollectionInterface::SupportedFeatures::MANAGEABLE |
//       CollectionInterface::SupportedFeatures::REMOVE|
      CollectionInterface::SupportedFeatures::ADD        );
}

bool FallbackPersonCollection::clear()
{
   QDir dir(d_ptr->m_Path);
   for (const QString& file : dir.entryList({"*.vcf"},QDir::Files))
      dir.remove(file);
   return true;
}

QByteArray FallbackPersonCollection::id() const
{
   return "fpc2";
}


void FallbackPersonCollectionPrivate::loadAsync()
{
   QDir d(m_Path);
   for (const QString& dir : d.entryList(QDir::AllDirs)) {
      PersonModel::instance()->addCollection<FallbackPersonCollection,QString,FallbackPersonCollection*>(m_Path+'/'+dir,q_ptr);
   }
}

#include "fallbackpersoncollection.moc"