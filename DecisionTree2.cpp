// DecisionTree2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

double threshold;
FILE *fout;

typedef struct node{
	int indexFitur;
	vector<pair<char, struct node*> > branch;
	char kelas;
	double errorRate;
	Dataset *dataset;
} Node;

double findGini(Dataset &dataset) {
	double jumlahData = dataset.getJumlahData();
	if (jumlahData > 0) {
		vector<double> jumlahMemberPerKelas;
		dataset.getJumlahMemberPerKelas(jumlahMemberPerKelas);
		int siz = dataset.getJumlahNilaiFitur(dataset.getJumlahFitur() - 1);
		double gini = 0.0;
		for (int i = 0; i < siz; i++) {
			double x = jumlahMemberPerKelas[i] / jumlahData;
			gini += x * x;
		}
		gini = 1 - gini;
		return gini;
	}
	else return 1;
}

void buatTree(Node &node, Dataset &dataset, char kelasParent = 0) {
	node.dataset = &dataset;

	if (dataset.getJumlahData() > 0) {
		node.kelas = dataset.getModusKelas();
	}
	else {
		node.kelas = kelasParent;
	}

	//mulai perhitungan gini
	int siz = dataset.getJumlahFitur()-1; //-1 karena yang terakhir itu klasifikasinya, bukan fitur
	double gini = findGini(dataset);
	//gini == 0 artinya semua data pada dataset memiliki kelas yang sama, atau jumlah datanya 0
	if (gini > 0 && dataset.getJumlahData() > threshold) {
		double maxGain = -1.0;
		int maxIndex;
		vector<Dataset*> datasetSplit;
		double jumlahData = dataset.getJumlahData();
		for (int i = 0; i < siz; i++) {
			int missingNum = dataset.split(datasetSplit, i);
			int ssiz = datasetSplit.size();
			double splitGini = 0.0;
			for (int j = 0; j < ssiz; j++) { //cari gini masing-masing dataset branch ( |Ti| / |T| * Gini(Ti)  )
				splitGini += (datasetSplit[j]->getJumlahData() / jumlahData) * findGini(*datasetSplit[j]);
			}
			double giniGain = gini - splitGini;

			if (giniGain > maxGain) {
				maxGain = giniGain;
				maxIndex = i;
			}
		}
		//split dataset berdasarkan atribut dengan information gain yang paling tinggi
		dataset.split(datasetSplit, maxIndex);
		node.indexFitur = dataset.getIndexKolomFitur(maxIndex);
		siz = datasetSplit.size();
		for (int i = 0; i < siz; i++) {
			Node *nd = new Node();
			node.branch.push_back(make_pair(dataset.getNilaiFitur(maxIndex, i), nd));
			//rekursif
			buatTree(*nd, *datasetSplit[i], node.kelas);
		}
	}
	else {
		node.branch.clear();
	}
}


void klasifikasi(Node *currentNode, const char *c, double *prob, double weight, char kelas = 0) {
	int siz = currentNode->branch.size();
	if (siz > 0) {
		bool found = false;
		for (int i = 0; i < siz; i++) {
			if (currentNode->branch[i].first == c[currentNode->indexFitur]) {
				found = true;
				klasifikasi(currentNode->branch[i].second, c, prob, weight);
				break;
			}
		}
		if (!found) {
			double branchWeight;
			double jumlahData = currentNode->dataset->getJumlahData();
			for (int i = 0; i < siz; i++) {
				branchWeight = weight * currentNode->branch[i].second->dataset->getJumlahData() / jumlahData;
				klasifikasi(currentNode->branch[i].second, c, prob, branchWeight);
			}
		}
	}
	else {
		prob[currentNode->dataset->getIndexKelas(currentNode->kelas)] += weight;
	}
}

void printTree(FILE* fout, Node* root, int tabNum = 0) {
	int siz = root->branch.size();
	vector<double> jumlahMemberPerKelas;
	if (siz > 0) {
		for (int i = 0; i < siz; i++) {
			for (int j = 0; j < tabNum; j++) {
				fputc('\t', fout);
			}
			fprintf_s(fout, "%d == %c ", root->indexFitur, root->branch[i].first);
			root->branch[i].second->dataset->getJumlahMemberPerKelas(jumlahMemberPerKelas);
			for (int j = 0; j < 2; j++) {
				fprintf_s(fout, "%lf : ", jumlahMemberPerKelas[j]);
			}
			fprintf_s(fout, "\n");
			printTree(fout, root->branch[i].second, tabNum + 1);
		}
	}
	else {
		for (int i = 0; i < tabNum; i++) {
			fputc('\t', fout);
		}
		fprintf_s(fout, "%s\n", root->kelas == 'e' ? "Edible" : "Poisonous");
	}
}

int _tmain(int argc, _TCHAR* argv[])
{

	fopen_s(&fout, "out3.txt", "w");

	Dataset *dataset;
	int jumlahData;
	int jumlahFitur;
	scanf_s("%d%d", &jumlahData, &jumlahFitur);
	vector<pair<int,vector<char>> > spesifikasiFitur;
	for (int i = 0; i <= jumlahFitur; i++) {
		vector<char> spesifikasiFiturI;
		int jumlahNilai;
		scanf_s("%d ", &jumlahNilai);
		for (int j = 0; j < jumlahNilai; j++) {
			char c;
			scanf_s("%c", &c);
			spesifikasiFiturI.push_back(c);
		}
		spesifikasiFitur.push_back(make_pair(i, spesifikasiFiturI));
	}
	scanf_s("%lf", &threshold);
	
	getchar();

	vector<char*> dataVector;
	for (int i = 0; i < jumlahData; i++) {
		char *c = new char[jumlahFitur + 1];
		for (int j = 0; j <= jumlahFitur; j++) {
			scanf_s("%c", &c[j]);
			getchar();
		}
		dataVector.push_back(c);
	}
	
	//Tree dibangun dengan menggunakan 60% data
	//Validasi dan pruning menggunakan 20% data
	//Testing menggunkan 20% data
	//semua mutually-exclusive
	vector<char*> trainingVector = vector<char*>(dataVector.begin(), dataVector.begin() + jumlahData * 3 / 5);
	vector<char*> validatingVector = vector<char*>(dataVector.begin() + trainingVector.size(), dataVector.begin() + (jumlahData * 4 / 5));
	vector<char*> testingVector = vector<char*>(dataVector.begin() + trainingVector.size() + validatingVector.size(), dataVector.end());
	dataset = new Dataset(spesifikasiFitur, trainingVector);

	Node rootNode;
	buatTree(rootNode, *dataset);


	//------------------------------------------------------------------------------------------------------------------------------------//

	fprintf_s(fout, "Prepruning threshold: %lf\n", threshold);
	printTree(fout, &rootNode);
	char *c = new char[jumlahFitur];
	int indexKelas = spesifikasiFitur.size() - 1;
	int jumlahKelas = spesifikasiFitur[indexKelas].second.size();
	double *prob = new double[jumlahKelas];
	int tepat = 0;

	for (int i = 0; i < jumlahData; i++) {
		for (int j = 0; j < jumlahKelas; j++) {
			prob[j] = 0.0;
		}
		klasifikasi(&rootNode, dataVector[i], prob, 1.0);
		double maxProb = -1.0;
		int maxIndex;
		for (int j = 0; j < jumlahKelas; j++) {
			if (prob[j] > maxProb) {
				maxIndex = j;
				maxProb = prob[j];
			}
		}
		char kelasAsli = dataVector[i][indexKelas];
		char kelasPrediksi = dataset->getCharKelas(maxIndex);
		for (int j = 0; j < jumlahFitur - 1; j++) {
			fprintf_s(fout, "%c ", dataVector[i][j]);
		}
		fprintf_s(fout, "\tKlasifikasi: %c -- Kelas asli: %c %s", kelasPrediksi, kelasAsli, kelasAsli == kelasPrediksi ? "\n" : "(SALAH)\n");
		tepat += kelasAsli == kelasPrediksi ? 1 : 0;
	}
	fprintf(fout, "Ketepatan: %d/%d (%.2lf%c)\n", tepat, jumlahData, (double)tepat / (double)jumlahData * 100.0, '%');
	

	delete(c);
	delete(prob);
	
	scanf_s("%*s");

	return 0;
}

