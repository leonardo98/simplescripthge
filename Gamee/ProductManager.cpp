#include "ProductManager.h"
#include "GameField.h"

ProductManager::Places ProductManager::_places;

ProductManager::ProductManager() {}

void ProductManager::Register(PlaceType pt, ProductPlace *place) {
	_places[pt] = place;
}

bool ProductManager::PlaceProduct(PlaceType pt, const std::string &product, float x) {
	if (_places[pt]->IsBusy()) {
		return false;
	}
	_places[pt]->SetProduct(product, x > _places[pt]->_pos.x);
	return true;
}

ProductPlace * ProductManager::GetPlaceProduct(PlaceType pt) {
	if (_places.find(pt) != _places.end() && !_places[pt]->IsBusy()) {
		return _places[pt];
	}
	return NULL;
}