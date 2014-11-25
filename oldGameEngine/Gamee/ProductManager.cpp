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

std::string ProductManager::ProductWant() {
	int r = rand() % 8;
	if (r == 0) {
		return "onion";
	} else if (r == 1) {
		return "beet";
	} else if (r == 2) {
		return "pumpkin";
	} else if (r == 3) {
		return "avocado";
	} else if (r == 4) {
		return "banana";
	} else if (r == 5) {
		return "peach";
	} else if (r == 6) {
		return "eggs_dodo";
	} else if (r == 7) {
		return "fluff";
	} else {
		assert(false);
		return "";
	}
}
