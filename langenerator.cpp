#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <cstdlib>

using namespace std;


const int infinito = 2134567890;


class language {

    vector<string> vstart; /// posibles comienzos
    map<string, vector<char> > vmiddle; /// posibles continuaciones


    /**
     * Funcion de hash para un string
     *
     * @param s  string de entrada
     *
     * @returns hash code de s
     */
    int hash( string s ) {

        int sol = 0;
        for ( string::iterator i = s.begin() ; i != s.end() ; ++i )
            sol = ( 5 * sol + (*i) ) % infinito;
        return sol;
    }


    /**
     * Lee una palabra (variable valida) o una secuencia de caracteres que no
     * se traducen (ej. ?!"{}[]... )
     *
     * @param ent  stream de entrada
     * @param sol  string destino
     *
     * @returns true si hay al menos un isblank() delante de la palabra
     */
    bool leer( istringstream& ent, string& sol ) {

        char c;
        ent >> noskipws >> c;
        bool res = isblank(c);

        while ( ent && isblank(c) )
            ent >> c;

        if (!ent) return false;

        sol.clear();
        sol.push_back( tolower(c) );

        if ( isalpha(c) || c == '_' ) {

            ent >> c;
            while ( ent && ( isalnum(c) || c == '_' ) ) {

                sol.push_back( tolower(c) );
                ent >> c;
            }

        } else {

            ent >> c;
            while ( ent && !( isalpha(c) || c == '_' || isblank(c) ) ) {

                sol.push_back( tolower(c) );
                ent >> c;
            }
        }

        if (ent)
            ent.unget();

        return res;
    }


    /**
     * Saca las letras repetidas de una palabra ( saca a partir de 3 repetidas )
     *
     * @param s  string de entrada
     *
     * @returns string de salida
     */
    string quitar_repetidos( string s ) {

        string sol;
        for ( string::iterator i = s.begin() ; i != s.end() ; ++i )
            if ( distance( s.begin(), i ) < 2 || !( *(i-1) == *i && *(i-2) == *i ))
                sol.push_back(*i);
        return sol;
    }


    /**
     * Pone los repetidos sacados por quitar_repetidos() de vuelta
     *
     * @param orig  string original (con los repetidos)
     * @param encr  encriptacion del string resultado de quitar_repetidos()
     *
     * @returns modificacion de @a encr con los repetidos en las "mismas" posiciones que @a origi
     */
    string recuperar_repetidos( string orig, string encr ) {

        string sol;
        int pos = 0;

        for ( string::iterator i = orig.begin() ; i != orig.end() ; ++i ) {

            if ( distance( orig.begin() , i ) > 1 && *(i-1) == *i && *(i-2) == *i ) {

                sol.push_back( sol[ sol.size()-1 ] );

            } else {

                if ( pos < encr.size() ) sol.push_back( encr[pos++] );
            }
        }

        if ( pos < encr.size() ) sol.push_back( encr[pos] );
        return sol;
    }


    public:


        /**
         * Default constuctor
         */
        language(){}


        /**
         * Constructor, calcula vstart y vmiddle
         *
         * @param file  archivo con las palabras del idioma base
         */
        language( string file ) {

            ifstream ent( file.c_str() );

            // read words
            vector<string> words;
            copy( istream_iterator<string>(ent), istream_iterator<string>(), back_inserter(words) );
            sort( words.begin(), words.end() );
            vector<string>::iterator it = unique( words.begin(), words.end() );
            words.resize( distance( words.begin(), it ) );

            // fill start & middle
            map< string, set<char> > start, middle;

            for ( vector<string>::iterator i = words.begin() ; i != words.end() ; ++i ) {

                if ( i->size() > 2 ) {

                    start[ i->substr(0,2) ].insert( (*i)[2] );
                    for ( int j = 0 ; j < i->size() - 2 ; j++ )
                        middle[ i->substr(j,2) ].insert( (*i)[j+2] );
                }
            }

            // fill vstart & vmiddle
            for ( map<string,set<char> >::iterator i = start.begin() ; i != start.end() ; ++i )
                vstart.push_back( i->first );

            for ( map<string,set<char> >::iterator i = middle.begin() ; i != middle.end() ; ++i )
                for ( set<char>::iterator j = i->second.begin() ; j != i->second.end() ; ++j )
                    vmiddle[ i->first ].push_back(*j);
        }


        /**
         * @param text  texto a traducir
         *
         * @returns texto traducido
         */
        string translate( string text ) {

            text.push_back(' ');
            istringstream ent(text);
            ostringstream sal;
            string r,s,t;
            bool b = leer(ent,r);

            while (ent){

//              cout << r << ' ' << boolalpha << b << endl;

                if ( ( isalpha(r[0]) || r[0] == '_' ) && r != "_" ) {

                    s = quitar_repetidos(r);
//                  cout << s << endl;

                    srand( hash(s) );
                    t = vstart[ rand() % vstart.size() ];

                    for ( int i = 0 ; i < s.size()-1 ; i++ )
                        if ( vmiddle[ t.substr(i,2) ].size() )
                            t.push_back( vmiddle[ t.substr(i,2) ][ rand() % vmiddle[ t.substr(i,2) ].size() ] );
                        else
                            t.push_back( t[ rand() % (i+2) ] );

//                  cout << r << ' ' << s << ' ' << t << ' ';
//                  cout << t << ' ';
                    t.resize( t.size() -  rand() % ( 3 - (r.size() < 3) ) );
                    t = recuperar_repetidos(r,t);
//                  cout << t << endl;
//                  cout << r << ' ' << s << ' ' << t << ' ';
//                  t = recuperar_repetidos(r,t);
//                  s = recuperar_repetidos(r,s);
//                  cout << t << ' ' << s<< endl;

                } else {
                    t = r;
                }

                if (b) sal << ' ';
                sal << t;
                b = leer(ent,r);
            }

            return sal.str();
        }
};


int main() {

    language L("sumerian.htm");
    string s;

    while (true) {

        cout << "Escriba el texto a traducir:" << endl;
        getline( cin, s );
        cout << L.translate(s) << endl;
    }
}

