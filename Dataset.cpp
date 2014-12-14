#include "stdafx.h"
#include <cmath>

Dataset::Dataset() {
	data = vector<char*>();
	dataWeight = vector<double>();
	data.clear();
	dataWeight.clear();
}

Dataset::Dataset(vector< pair<int ,vector<char> > > &spesifikasiFitur, vector<char*> &data)
{
	this->spesifikasiFitur = spesifikasiFitur;
	this->data = data;
	jumlahDataBulat = data.size();
	jumlahDataPecahan = 0.0;
	dataWeight = vector<double>(jumlahDataBulat, 1.0);
}


Dataset::~Dataset()
{
}

void Dataset::setSpesifikasiFitur(vector<pair<int, vector<char>> > &spesifikasiFitur) {
	this->spesifikasiFitur = spesifikasiFitur;
}

double Dataset::getJumlahData() {
	return (double)jumlahDataBulat + jumlahDataPecahan;
}


int Dataset::getJumlahFitur() {
	return spesifikasiFitur.size();
}

int Dataset::getJumlahNilaiFitur(int indexFitur) {
	return spesifikasiFitur[indexFitur].second.size();
}

char Dataset::getNilaiFitur(int indexFitur, int index) {
	return spesifikasiFitur[indexFitur].second[index];
}


void Dataset::getJumlahMemberPerKelas(vector<double> &jumlahMemberPerKelas) {
	int siz = data.size();
	int indexAtributKelas = spesifikasiFitur.size() - 1;
	int ssiz = spesifikasiFitur[indexAtributKelas].second.size();
	jumlahMemberPerKelas.clear();
	for (int i = 0; i < ssiz; i++) {
		jumlahMemberPerKelas.push_back(0.0);
	}
	for (int i = 0; i < siz; i++) {
		int index = 0;
		for (int j = 0; j < ssiz; j++) {
			if (data[i][spesifikasiFitur[indexAtributKelas].first] == spesifikasiFitur[indexAtributKelas].second[j]) {	
				jumlahMemberPerKelas[j] += dataWeight[i];
				break;
			}
		}
	}
}

int Dataset::split(vector<Dataset*> &datasetSplit, int indexFitur) {
	datasetSplit.clear();
	//buat spesifikasi fitur baru (spesifikasi fitur lama tanpa fitur yang dipake untuk split)
	int siz = spesifikasiFitur.size();
	vector<pair<int,vector<char>> > *spesifikasiFiturBaru = new vector<pair<int,vector<char>> >;
	for (int i = 0; i < siz; i++) {
		if (indexFitur != i) {
			spesifikasiFiturBaru->push_back(spesifikasiFitur[i]);
		}
	}
	siz = spesifikasiFitur[indexFitur].second.size();
	for (int i = 0; i < siz; i++) {
		Dataset *ds = new Dataset();
		ds->setSpesifikasiFitur(*spesifikasiFiturBaru);
		datasetSplit.push_back(ds);
	}
	
	//kalau ada nilai yang hilang, simpan dulu lalu masukin belakangan
	vector <int> indexNilaiHilang;
	//iterasi data, kelompokkan berdasarkan nilai dari atribut yang dipilih
	int ssiz = siz; //ssiz = spesifikasiFitur[indexFitur].size()
	siz = data.size();
	for (int i = 0; i < siz; i++) {
		int index = -1;
		for (int j = 0; j < ssiz; j++) {
			if (data[i][spesifikasiFitur[indexFitur].first] == spesifikasiFitur[indexFitur].second[j]) {
				index = j;
				break;
			}
		}
		if (index != -1) {
			datasetSplit[index]->pushData(data[i], dataWeight[i]);
		}
		else {
			indexNilaiHilang.push_back(i);
		}
	}
	
	double *missingWeight = new double[ssiz];
	for (int i = 0; i < ssiz; i++) {
		missingWeight[i] = datasetSplit[i]->getJumlahData() / getJumlahData();
	}

	//sebar data dengan nilai yang hilang ke semua dataset anak
	siz = indexNilaiHilang.size();
	for (int i = 0; i < siz; i++) {
		for (int j = 0; j < ssiz; j++) {
			datasetSplit[j]->pushData(data[indexNilaiHilang[i]], missingWeight[j]);
		}
	}

	delete (missingWeight);
	//proses "splitting" selesai, return jumlah data yang nilai atributnya hilang
	return siz;
}

void Dataset::pushData(char *data, double weight) {
	this->data.push_back(data);
	dataWeight.push_back(weight);
	jumlahDataBulat += (int)floor(weight);
	jumlahDataPecahan += weight - (int)floor(weight);
}


char Dataset::getKlasifikasi(int index) {
	return data[index][spesifikasiFitur[spesifikasiFitur.size() - 1].first];
}

char Dataset::getModusKelas() {
	vector<char> spesifikasiKelas = spesifikasiFitur[spesifikasiFitur.size() - 1].second;
	vector<double> jumlahMemberPerKelas;
	getJumlahMemberPerKelas(jumlahMemberPerKelas);
	double max = -1.0;
	char kelasModus;
	int siz = jumlahMemberPerKelas.size();
	for (int i = 0; i < siz; i++) {
		if (jumlahMemberPerKelas[i] > max) {
			max = jumlahMemberPerKelas[i];
			kelasModus = spesifikasiKelas[i];
		}
	}
	return kelasModus;
}

int Dataset::getIndexKolomFitur(int index) {
	return spesifikasiFitur[index].first;
}

char Dataset::getCharKelas(int index) {
	int indexKolomKelas = spesifikasiFitur.size() - 1;
	int siz = spesifikasiFitur[indexKolomKelas].second.size();
	if (index > siz || index < 0) {
		return '?';
	}
	return spesifikasiFitur[indexKolomKelas].second[index];
}

int Dataset::getIndexKelas(char c) {
	int indexKolomKelas = spesifikasiFitur.size() - 1;
	int siz = spesifikasiFitur[indexKolomKelas].second.size();
	for (int i = 0; i < siz; i++) {
		if (c == spesifikasiFitur[indexKolomKelas].second[i])
			return i;
	}
	return -1;
}
