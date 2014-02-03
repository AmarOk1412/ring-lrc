/****************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                               *
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
#ifndef TRANSITIONAL_CONTACT_BACKEND
#define TRANSITIONAL_CONTACT_BACKEND

#include "abstractcontactbackend.h"

#include "typedefs.h"

///Contact backend for new unsaved contacts
class LIB_EXPORT TransitionalContactBackend : public AbstractContactBackend {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:

   virtual ~TransitionalContactBackend();

   virtual bool load();
   virtual bool reload();
   virtual bool saveContact(const Contact* contact);

   ///Edit 'contact', the implementation may be a GUI or somehting else
   virtual void        editContact       ( Contact*       contact     );
   ///Add a new contact to the backend
   virtual void        addNewContact     ( Contact*       contact     );

   ///Add a new phone number to an existing contact
   virtual void addPhoneNumber( Contact*       contact , PhoneNumber* number );

   SupportedFeatures supportedFeatures() const;

   //Singleton
   static AbstractContactBackend* instance();

private:
   explicit TransitionalContactBackend(QObject* parent = nullptr);
   static AbstractContactBackend* m_spInstance;

};


#endif