#ifndef MYENGINE_PRODUCTMANAGER_H
#define MYENGINE_PRODUCTMANAGER_H

#include "ProductPlace.h"

class ProductManager {
public:
	ProductManager();
	static void Register(PlaceType pt, ProductPlace *place);
	static bool PlaceProduct(PlaceType pt, const std::string &product, float x);
	static ProductPlace * GetPlaceProduct(PlaceType pt);
	static std::string ProductWant();
private:
	typedef std::map<PlaceType, ProductPlace *> Places;
	static Places _places;
};

#endif//MYENGINE_PRODUCTMANAGER_H