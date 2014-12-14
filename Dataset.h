#pragma once
class Dataset
{
private:
	//spesifikasiFitur sudah menyimpan spesifikasi kelas juga
	vector<pair<int,vector<char>> > spesifikasiFitur;
	vector<char*> data;
	vector<double> dataWeight;
	int jumlahDataBulat;
	double jumlahDataPecahan;
public:
	Dataset();
	Dataset(vector<pair<int ,vector<char>> > &spesifikasiFitur, vector<char*> &data);
	~Dataset();
	void setSpesifikasiFitur(vector<pair<int,vector<char>> > &spesifikasiFitur);
	double getJumlahData();
	int getJumlahFitur();
	int getJumlahNilaiFitur(int indexFitur);
	char getNilaiFitur(int indexFitur, int index);
	int getIndexKolomFitur(int index);
	void getJumlahMemberPerKelas(vector<double> &jumlahMemberPerKelas);
	int split(vector<Dataset*> &datasetSplit, int indexFitur);
	void pushData(char* data, double dataWeight);
	char getKlasifikasi(int index);
	char getCharKelas(int index);
	int getIndexKelas(char c);
	char getModusKelas();
};

