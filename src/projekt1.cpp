//Projekt 1 - Prosta gra ekonomiczna (18.11.2024)
#include <iostream>
#include <cmath>
#include <math.h>
#include <memory> //smart pointers

using namespace std;
const int MAX_PRACOWNICY = 100; //Maksymalna liczba zatrudnionych pracownikow
const int N = 5; // Liczba miesiecy do obliczenia sredniej wartosci spolki
static const int X = 12; // Maksymalny czas splaty kredytu w miesiacach
static const double M = 3.5; // Maksymalna wartosc zadluzenia wzgledem wartosci firmy
const int MAX_KREDYTY = 10; // Zmienna globalna określająca maksymalną liczbę kredytów

class Pracownik
{
protected:
    string imie;
    const double wynagrodzenie;
public:
    Pracownik(string name, double salary) : imie(name), wynagrodzenie(salary) {}

    virtual void print() = 0;
    Pracownik (const Pracownik&) = default;
    Pracownik& operator=(const Pracownik&) = default;
    Pracownik (Pracownik&&) = default;
    Pracownik& operator=(Pracownik&&) = default;
    virtual ~Pracownik() = default;
    const double getWynagrodzenie()
    {
        return wynagrodzenie;
    }
};

class Inzynier : public Pracownik
{
private:
    // static const unsigned int CI = 88;
    string wydzial;
public:
    Inzynier(string im, string faculty) : Pracownik(im, 10.), wydzial(faculty) {}

    void print() override
    {
        cout << "Inzynier: ";
        cout << "Imie Inzyniera: " << imie << ", Wydzial Inzyniera: " << wydzial << ", Wynagrodzenie: " << wynagrodzenie << endl;
        // cout << "Wydajnosc: " << CI << endl;
    }
};

class Magazynier : public Pracownik
{
private:
    // static const unsigned int CMag = 45;
    bool obsl_widl;
public:
    Magazynier(string im, bool t_or_f) : Pracownik(im, 5.), obsl_widl(t_or_f) {}

    void print() override
    {
        cout << "Magazynier: ";
        cout << "Imie Magazyniera: " << imie << ", Obsluga wozka: " << obsl_widl << ", Wynagrodzenie: " << wynagrodzenie << endl;
        // cout << "Wydajnosc: " << CMag << endl;
    }
};

class Marketer : public Pracownik
{
private:
    // static const unsigned int CMkt = 67;
    unsigned int follows;
public:
    Marketer(string im, unsigned int obserwacje) : Pracownik(im, 7.5), follows(obserwacje) {}

    void print() override
    {
        cout << "Marketer: ";
        cout << "Imie Marketera: " << imie << ", Follows Marketera: " << follows << ", Wynagrodzenie: " << wynagrodzenie << endl;
        // cout << "Wydajnosc: " << CMkt << endl;
    }
};

class Robotnik : public Pracownik
{
private:
    // static const unsigned int CR = 21;
    double but;
public:
    Robotnik(string im, double shoe) : Pracownik(im, 2.5), but(shoe) {}

    void print() override
    {
        cout << "Robotnik: ";
        cout << "Imie Robotnika: " << imie << ", Rozmiar buta Robotnika: " << but << ", Wynagrodzenie: " << wynagrodzenie << endl;
        // cout << "Wydajnosc: " << CR << endl;
    }
};

struct Kredyt 
{
    double kwota;
    int okresSplaty;
    double rata;
    double pozostaleZadluzenie;
};

class Firma
{
private:
    static const unsigned int CI = 88;
    static const unsigned int CMag = 45;
    static const unsigned int CMkt = 67;
    static const unsigned int CR = 21;
    unique_ptr<Pracownik> pracownicyTablica[MAX_PRACOWNICY]; // Tablica wskaźników na obiekty Pracownik
    int liczbaPracownikow = 0;
    double stanKonta = 100; //kwota na start
    double przychod;
    double magazyn = 0;
    double przychody[N] = {0};
    int przychodyIndex = 0; //Index do iterowania przychody[N]
    double wartoscSpolki = 0;
    double zadluzenie = 0; // Aktualne zadluzenie, 1ile jeszcze nalezy oddac, potrzebne do sprawdzenia czy wygralismy gre
    Kredyt kredyty[MAX_KREDYTY]; //tablica wartosci typu Kredyt
    int liczbaKredytow = 0; // Liczba obecnie aktywnych kredytow
public:
    void dodajPracownika(unique_ptr<Pracownik> pracownik) 
    {
        if (liczbaPracownikow < MAX_PRACOWNICY)
        {
            pracownicyTablica[liczbaPracownikow] = move(pracownik); //move przekazuje wlasnosc smart pointera do tablicy
            liczbaPracownikow++;
        }
        else
        {
            cout << "MAX_PRACOWNICY osiagniety" << endl;
        }
    }

    void drukuj_pracownikow() {
        if (liczbaPracownikow == 0) 
        {
            cout << "Nie masz jeszcze Pracownikow" << endl;
        } 
        else 
        {
            cout << "Obecnie masz: " << liczbaPracownikow << " pracownikow w swojej firmie" << endl;
            cout << "Twoi pracownicy: " << endl;
            for (int i = 0; i < liczbaPracownikow; ++i) {
                pracownicyTablica[i]->print();
            }
        }
    }

    template <typename T>
    double pracownicyTypu(unsigned int C) 
    {
        unsigned int liczbaPracownikowTypu = 0;
        for (int i = 0; i < liczbaPracownikow; ++i) {
            // Sprawdzamy, czy pracownik jest typu T (np. Inzynier, Magazynier)
            if (dynamic_cast<T*>(pracownicyTablica[i].get())) { 
                liczbaPracownikowTypu++;
            }
        }
        return liczbaPracownikowTypu*C;
    }

    void oblicz_przychod()
    {
        double cenaProduktu = pracownicyTypu<Inzynier>(CI);
        double pojemnoscMagazynu = pracownicyTypu<Magazynier>(CMag);
        double popyt = pracownicyTypu<Marketer>(CMkt);
        double liczbaProduktowTeoria = pracownicyTypu<Robotnik>(CR);

        double wyprodukowane = coMniejsze(pojemnoscMagazynu, liczbaProduktowTeoria);
        wyprodukowane += magazyn; //Dodanie produktow z magazynu do dostepnych na sprzedaz
        magazyn = 0;
        double sprzedane = coMniejsze(popyt, wyprodukowane);
        przychod = sprzedane*cenaProduktu;
        if(sprzedane<wyprodukowane)
        {
            magazyn = wyprodukowane - sprzedane;
        }

        przychody[przychodyIndex] = przychod;
        przychodyIndex++;
        if(przychodyIndex==N)
        {
            przychodyIndex = 0;
        }

        cout << "Przychod: " << przychod << endl;
        cout << "Wyprodukowane: " << wyprodukowane << endl;
        cout << "Sprzedane: " << sprzedane << endl;
        cout << "Magazyn: " << magazyn << endl;

        stanKonta += przychod;
    }

    double coMniejsze(double a, double b) //Zwraca mniejsza wartosc z dwoch
    {
        if(a<=b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }

    double oblicz_wartosc_spolki() 
    {
        double suma = 0;
        for (int i = 0; i < N; ++i) 
        {
            suma += przychody[i];
        }
        wartoscSpolki = suma / N;
        // cout << "Wartosc spolki (sredni przychod z ostatnich " << N << " miesiecy): " << wartoscSpolki << endl;

        return wartoscSpolki;
    }

    void wez_kredyt(double kwota, int czas_splaty)
    {
        if (liczbaKredytow >= MAX_KREDYTY) 
        {
            cout << "Wykorzystales maksymalna ilosc kredytow !!!" << endl;
            return;
        }
        if (czas_splaty > X) 
        {
            cout << "Limit czasu splaty: " << X << " miesiecy" << endl;
            return;
        }
        oblicz_wartosc_spolki(); 
        if (zadluzenie + kwota > M * wartoscSpolki) 
        {
            cout << "Za wysoka kwota kredytu, Maksymalna kwota kredytu to: " << M * wartoscSpolki << endl;
            return;
        }
        double odsetki = (0.05 + 0.05 * czas_splaty) * kwota; // Prosty model odsetek
        double calkowitaKwota = kwota + odsetki;
        double miesiecznaRata = calkowitaKwota/czas_splaty;

        kredyty[liczbaKredytow].kwota = kwota;
        kredyty[liczbaKredytow].pozostaleZadluzenie = calkowitaKwota;
        kredyty[liczbaKredytow].okresSplaty = czas_splaty;
        kredyty[liczbaKredytow].rata = miesiecznaRata;
        zadluzenie += calkowitaKwota;
        stanKonta += kwota; //aktualizacja stanu konta
        liczbaKredytow++;

        cout << "Wziales kredyt: " << kwota << ", Czas splaty: " << czas_splaty << " miesiecy" << ", Miesieczna rata: " << miesiecznaRata << endl;
        cout << "Aktualny stan konta: " << stanKonta <<", Do splaty z odsetkami: " << calkowitaKwota << endl;
    }

    double get_stanKonta()
    {
        return stanKonta;
    }

    void zaplac_wynagrodzenie()
    {
        double sumaWyplat = 0; //Ile trzeba zaplacic na kazdemu na poczatku miesiaca
        for (int i = 0; i<liczbaPracownikow; ++i) {
            sumaWyplat += pracownicyTablica[i]->getWynagrodzenie();
        }

        if (stanKonta >= sumaWyplat) 
        {
            stanKonta -= sumaWyplat;
            cout << "Wyplata udana, Wyplacono: " << sumaWyplat << endl;
        } 
        else 
        {
            cout << "Brak wystarczajacych srodkow do zrobienia wyplat, Stan konta: " << stanKonta << ", Wymagane: " << sumaWyplat << endl;
        }
    }

    void splac_raty() 
    {
        for (int i = 0; i < liczbaKredytow; ++i) 
        {
            if (kredyty[i].okresSplaty > 0) {
                if (stanKonta >= kredyty[i].rata) 
                {
                    stanKonta -= kredyty[i].rata;
                    kredyty[i].pozostaleZadluzenie -= kredyty[i].rata;
                    kredyty[i].okresSplaty--;

                    zadluzenie -= kredyty[i].rata;

                    cout << "Splacono rate kredytu " << i + 1 << ": " << kredyty[i].rata
                         << ", Pozostaly okres kredytu: " << kredyty[i].okresSplaty << " miesiecy" << endl;
                    //cout << "Aktualny stan konta: " << stanKonta << endl;

                    if (kredyty[i].okresSplaty == 0) 
                    {
                        cout << "Kredyt " << i + 1 << " zostal splacony" << endl;
                        // Reset kredytu
                        kredyty[i].kwota = 0;
                        kredyty[i].pozostaleZadluzenie = 0;
                        kredyty[i].okresSplaty = 0;
                        kredyty[i].rata = 0;
                    }
                } 
                else 
                {
                    cout << "Brak srodkow do splaty kredytu: " << i + 1 << endl;
                }
            }
        }
    }

    void sprawdzStanGry()
    {
        if (stanKonta < 0)
        {
            cout << "\n!!! Przegrales gre: Firma zbankrutowala !!!" << endl;
            exit(0);
        }
        else if ((wartoscSpolki - zadluzenie) > 10000)
        {
            cout << "\n!!! Wygrales gre: Firma osiagnela sukces finansowy !!!" << endl;
            exit(0);
        }
        else
        {
            cout << "Graj dalej" << endl;
            //cout << "Gra toczy sie dalej, Stan konta: " << stanKonta << ", Wartosc spolki: " << wartoscSpolki << endl;
        }
    }

};

class Gra
{
private:
    Firma firma;
    bool stan = 1;
    int miesiac = 0;
public:
    void graStart() 
    {
        cout << "Gra rozpoczeta, komendy do wyboru: " << endl;
        cout << "lp - wylistuj pracownikow " << endl;
        cout << "zinz - zatrudnij inzyniera " << endl;
        cout << "zmag - zatrudnij magazyniera " << endl;
        cout << "zmkt - zatrudnij marketera " << endl;
        cout << "zrob - zatrudnij robotnika " << endl;
        cout << "kred - wez kredyt (w nastepnym kroku, podaj kwote i czas splaty) " << endl;
        cout << "kt - zakoncz ture i wyswietl stan firmy na poczatku nastepnego miesiaca " << endl;
        cout << "stan - sprawdz aktualny stan konta" << endl;
        cout << "exit - zakoncz gre, wyjscie z gry " << endl;;
    }

    void akcjaGracza()
    {
        string command;
        cout << "\nMiesiac nr: " << miesiac << endl;
        while (true) 
        {
            cout << "Podaj komenda: ";
            cin >> command;

            if (command == "lp") 
            {
                firma.drukuj_pracownikow();
            }
            else if (command == "zinz") 
            {
                string name = "Hubert";
                string faculty = "MEiL";
                firma.dodajPracownika(make_unique<Inzynier>(name, faculty));
                cout << "Zatrudniono Inzyniera " << endl;
            }
            else if (command == "zmag") 
            {
                string name = "Jan";
                bool obsl_widl = true;
                firma.dodajPracownika(make_unique<Magazynier>(name, obsl_widl));
                cout << "Zatrudniono Magazyniera " << endl;
            }
            else if (command == "zmkt") 
            {
                string name = "Tomek";
                unsigned int follows = 5675;
                firma.dodajPracownika(make_unique<Marketer>(name, follows));
                cout << "Zatrudniono Marketera " << endl;
            }
            else if (command == "zrob") 
            {
                string name = "Borys";
                double shoe = 44.5;
                firma.dodajPracownika(make_unique<Robotnik>(name, shoe));
                cout << "Zatrudniono Robotnika " << endl;
            }
            else if (command == "kred") 
            {
                double kwota;
                int czas_splaty;
                cout << "Podaj kwote kredytu: ";
                cin >> kwota;
                cout << "Podaj czas splaty w miesiacach: ";
                cin >> czas_splaty;
                firma.wez_kredyt(kwota, czas_splaty);
            }
            else if (command == "stan")
            {
                cout << "Twoj obecny stan konta to: " << firma.get_stanKonta() << endl;
            }
            else if (command == "kt")
            {
                cout << "Koniec tury" << endl;
                miesiac++;
                cout << "\nKolejna tura, miesiac: " << miesiac << endl;
                firma.oblicz_przychod();
                cout << "Wartosc spolki (sredni przychod z ostatnich " << N << " miesiecy): " << firma.oblicz_wartosc_spolki() << endl;
                firma.zaplac_wynagrodzenie();
                firma.splac_raty();
                cout << "Twoj obecny stan konta to: " << firma.get_stanKonta() << endl;
                firma.sprawdzStanGry();
            }
            else if (command == "exit") 
            {
                cout << "\nWyjscie z gry" << endl;
                break;
            }
            else 
            {
                cout << "Nieznana komenda!" << endl;
            }
        }
    }
};

int main()
{
    Gra gra;
    gra.graStart();
    gra.akcjaGracza();
    return 0;
}
