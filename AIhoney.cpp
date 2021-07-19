#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME honey4

// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.

struct PLAYER_NAME : public Player {    

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
    const int I[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };
    const int J[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };

  typedef vector<int> VE;

  map<int, int> kind; // For cars: 0 -> random, 1 -> Top.
  
  bool es_meva_ciutat(int i, int j){ //per comprovar si en pos ij hi ha una ciutat meva o no
      
      if(cell(i,j).owner == me()) return true;
      else return false;
}
 bool pot_moure_w(int player,int i, int j){ //vull saber si es pot moure el w mirant el seu voltant (k no pugui caminar, que no hi hagi cotxe o soldat aliat al costat
     
     int p1 = cell(i,j).id;
     if(cell(i,j).type == Water or cell(i,j).type == Station or cell(i,j).type == Wall ) return false;
     if(p1!=-1 and unit(p1).type== Car)return false;
     if(p1!=-1 and unit(p1).player == player)return false;
     return true;
     
     
}
bool pot_moure_cgas(int player,int i, int j){ //vull saber si es pot moure el w mirant el seu voltant (k no pugui caminar, que no hi hagi cotxe o soldat aliat al costat
     
     int p1 = cell(i,j).id;
     if(cell(i,j).type!=Road and cell(i,j).type!= Station) return false;
     if(p1!=-1 and unit(p1).type== Car)return false;
     if(p1!=-1 and unit(p1).player == player)return false;
     
     return true;
     
     
}
bool pot_moure_croad(int player,int i, int j){ //vull saber si es pot moure el w mirant el seu voltant (k no pugui caminar, que no hi hagi cotxe o soldat aliat al costat
     
     int p1 = cell(i,j).id;
     if(cell(i,j).type!=Road and cell(i,j).type!= Desert and cell(i,j).type!= Station) return false;
     if(p1!=-1 and unit(p1).type== Car)return false;
     if(p1!=-1 and unit(p1).player == player)return false;
     
     return true;
     
     
}

 bool pot_moure_c(int player,int i, int j, vector<Pos>& pc,int id){ //vull saber si es pot moure el w mirant el seu voltant (k no pugui caminar, que no hi hagi cotxe o soldat aliat al costat
     
     //if(cell(unit(id).pos).type == Desert)return true; //si esta al desert perk ha sortit de la carretera per matar a algu ha de poder tornar a entrar a la carretera i moure's
     
     int p1 = cell(i,j).id;
     if(cell(i,j).type!=Road and cell(i,j).type!= Station) return false;
     if(p1!=-1 and unit(p1).type== Car)return false;
     if(p1!=-1 and unit(p1).player == player)return false;
     int c=cars(me()).size();
     for(int r=0;r<c;++r){
         int pi = pc[r].i;
         int pj = pc[r].j;
         if(pi==i and pj == j)return false;
    }
     return true;
     
     
}
bool no_ataca_w(int id){// retorna si no ataca=true i si retorna fals, fa command
    Pos p=unit(id).pos;
    int player=unit(id).player;
    for(int k = 0 ;k < 8;++k){
        int pi = p.i + I[k];
        int pj = p.j + J[k];
        if(pos_ok(pi,pj)){
            int id2= cell(pi,pj).id;
            if(id2>0 and player!= unit(id2).player/*and unit(id2).type != Car*/){
                command(id,Dir(k));
                return false;
            }
        }
    }
    return true;
}
bool no_ataca_c(int id){// retorna si no ataca=true i si retorna fals, fa command
    Pos p=unit(id).pos;
    int player=unit(id).player;
    for(int k = 0 ;k < 8;++k){
        
        int pi = p.i + I[k];
        int pj = p.j + J[k];
        //cerr<<pi<<"<-pi pj->"<<pj<<endl;
        if(pos_ok(pi,pj)){
            int id2= cell(pi,pj).id;
            if(id2 != -1){ 
                //cerr<<"PRIMER "<< id2<<" player and me "<< player<<me()<< endl;
                if(player != unit(id2).player){ 
                    //cerr<<"SEGON "<< player<<"<-player ->me "<<me()<<endl;
                    if(unit(id2).type == Warrior and cell(unit(id2).pos).type != City ){
                        //cerr<<"Ha trobat un enemic adjacent per tant ataca i no fa bfs"<<endl;
                        command(id,Dir(k));
                        return false;
            }
        }}
        }
    }
    return true;
}

bool es_adjacent_aigua(Pos posi){
    
    for(int k=0; k<8;++k){
        int pi = posi.i + I[k];
        int pj = posi.j + J[k];
        if(pos_ok(pi,pj)){
            if(cell(pi,pj).type==Water)return true;
        }
        
    }
    return false;
}

void BFS_warrior(int id){
    Pos posi=unit(id).pos;
    int player = unit(id).player;
    vector<vector<bool> > mapa(60,vector<bool>(60,false));
    vector<vector<Pos> > m(60, vector<Pos>(60,Pos(-1,-1))); 
    
    Pos fi=Pos(-1,-1);
    Pos seg;
    seg.i=posi.i;
    seg.j=posi.j; 
    Pos actual;
    
    //cerr <<"principi "<< seg.i << " " <<seg.j <<  endl;
    
    mapa[posi.i][posi.j]=true; //posem la posi actual en visitat
    queue<Pos> cua;
    cua.push(posi);
    Pos aux;

    while(not cua.empty()){ //mentre no estigui buida, no hem trobat el k volem i/o tots elem visitats
        actual = cua.front();
        cua.pop();
        
        //cerr<<"aqui el -1? "<<endl;
        int city_owner = cell(actual.i,actual.j).owner;
        if(city_owner!= -1  and city_owner!= player ) { //si hi ha una ciutat i no es meva
            //cerr<<"aqui no pot ser -1  "<<endl;
            while(not cua.empty())cua.pop();//buidem la cua per sortir del bucle
            fi.i=actual.i; //guardem la posi de la city
            fi.j=actual.j;
            //ALERT if (size>unit(id).water)BFS_water(id); //adjacent(int id, celltype t)
                        
        }    
        else{ //si encara no trobem la ciutat
            //cerr<<"aqui el -1? nose  "<<endl;
            for(int k=0; k<8; ++k){ //Mirem el voltant de la posicio si hi ha una pos amb prioritat devant les altres
                aux.i = actual.i + I[k];
                aux.j = actual.j + J[k];

                //cerr<<"aqui el -1? tu creus? "<<endl; //aqui tenim el problema del -1
                if(pos_ok(aux) and pot_moure_w(player,aux.i,aux.j) and not mapa[aux.i][aux.j]){
                   //cerr<<"aqui el -1? ho accepta "<<endl;
                        mapa[aux.i][aux.j]=true;
                        cua.push(aux); 
                        m[aux.i][aux.j].i = actual.i;//afegim la posicio a la cua, en el cas que hi hagues una city la cua estarà buida i afegirem nomes aquesta posicio(prioritaria)
                        m[aux.i][aux.j].j = actual.j;
                        
                    }
            }  
        }
    }
    stack<Pos> s;
    if(fi != Pos(-1,-1)){ //no entraaaaaaaaaa
        //cerr << "if fi" << endl;
        // cerr << fi.i << " " << fi.j << " fi " <<endl;
        //cerr << posi.i << " " << posi.j << "posi " <<endl;
        if(fi==posi)s.push(fi); //linia perque sino si coincidia, al guardar a seg el s.top() feia un segmentation xk no hi havia res a s
        while(fi != posi){
            //cerr<<"if fi!=posi"<<endl;
            int ax;  
            s.push(fi);
            ax = m[fi.i][fi.j].i;
            fi.j = m[fi.i][fi.j].j;
            fi.i=ax;
        }
        //cerr<< "top.i "<<s.top().i<<" top.j "<< s.top().j<<endl;
        //cerr<<"o fi==posi o fora bucle"<<endl;
        seg.i = s.top().i;
        seg.j = s.top().j;
        //cerr<< "seg.i "<<seg.i<<" seg.j "<< seg.j<<endl;
        
        
    }
    //cerr << seg.i << " " <<seg.j <<  endl;
    for(int k =0 ; k < 8 ; ++k){ //com ja hem trobat la posició a la que volem mourens, busquem quina direccio es        
        Pos p(posi+ Dir(k));
        int pi=p.i;
        int pj=p.j;
        if(pi== seg.i and pj == seg.j){
            command(id,Dir(k));
            
           //cerr << p.i << " " << p.j << " k= "<<k<<  endl;
           k=8;
        }
    }
    
    
}
void BFS_car(int id, vector<Pos>& pc,int it){
    Pos posi=unit(id).pos;
    int player = unit(id).player;
    vector<vector<bool> > mapa(60,vector<bool>(60,false));
    vector<vector<Pos> > m(60, vector<Pos>(60,Pos(-1,-1))); 
    
    Pos fi=Pos(-1,-1);
    Pos seg;
    seg.i=posi.i;
    seg.j=posi.j; 
    Pos actual;
    
    //cerr <<"principi "<< seg.i << " " <<seg.j <<  endl;
    
    mapa[posi.i][posi.j]=true; //posem la posi actual en visitat
    queue<Pos> cua;
    cua.push(posi);
    Pos aux;
    while(not cua.empty()){ //mentre no estigui buida, no hem trobat el k volem i/o tots elem visitats
        actual = cua.front();
        cua.pop();
        
        //cerr<<"aqui el -1? "<<endl;
        int warrior = cell(actual.i,actual.j).id;
        if(warrior!= -1  and unit(warrior).player!= player and unit(warrior).type!= Car ) { //si hi ha una ciutat i no es meva
            //cerr<<"aqui no pot ser -1  "<<endl;
            while(not cua.empty())cua.pop();//buidem la cua per sortir del bucle
            fi.i=actual.i; //guardem la posi de la city
            fi.j=actual.j;
            //ALERT if (size>unit(id).water)BFS_water(id); //adjacent(int id, celltype t)
                        
        }    
        else{ //si encara no trobem la ciutat
            //cerr<<"aqui el -1? nose  "<<endl;
            for(int k=0; k<8; ++k){ //Mirem el voltant de la posicio si hi ha una pos amb prioritat devant les altres
                aux.i = actual.i + I[k];
                aux.j = actual.j + J[k];

                //cerr<<"aqui el -1? tu creus? "<<endl; //aqui tenim el problema del -1
                if(pos_ok(aux) and pot_moure_c(player,aux.i,aux.j,pc,id) and not mapa[aux.i][aux.j]){
                   //cerr<<"aqui el -1? ho accepta "<<endl;
                    mapa[aux.i][aux.j]=true;
                    cua.push(aux); 
                    m[aux.i][aux.j].i = actual.i;//afegim la posicio a la cua, en el cas que hi hagues una city la cua estarà buida i afegirem nomes aquesta posicio(prioritaria)
                    m[aux.i][aux.j].j = actual.j;
                        
                }
            }  
        }
    }
    stack<Pos> s;
    if(fi != Pos(-1,-1)){ //no entraaaaaaaaaa
        //cerr << "if fi" << endl;
        // cerr << fi.i << " " << fi.j << " fi " <<endl;
        //cerr << posi.i << " " << posi.j << "posi " <<endl;
        if(fi==posi)s.push(fi); //linia perque sino si coincidia, al guardar a seg el s.top() feia un segmentation xk no hi havia res a s
        while(fi != posi){
            //cerr<<"if fi!=posi"<<endl;
            int ax;  
            s.push(fi);
            ax = m[fi.i][fi.j].i;
            fi.j = m[fi.i][fi.j].j;
            fi.i=ax;
        }
        //cerr<< "top.i "<<s.top().i<<" top.j "<< s.top().j<<endl;
        //cerr<<"o fi==posi o fora bucle"<<endl;
        seg.i = s.top().i;
        seg.j = s.top().j;
        //cerr<< "seg.i "<<seg.i<<" seg.j "<< seg.j<<endl;
        
        
    }
    //cerr << seg.i << " " <<seg.j <<  endl;
    for(int k =0 ; k < 8 ; ++k){ //com ja hem trobat la posició a la que volem mourens, busquem quina direccio es        
        Pos p(posi+ Dir(k));
        int pi=p.i;
        int pj=p.j;
        if(pi== seg.i and pj == seg.j){
            pc[it].i = seg.i;
            pc[it].j = seg.j;
            command(id,Dir(k));
            k=8;
        }
    } 
}
void BFS_gas(int id){
    Pos posi=unit(id).pos;
    int player = unit(id).player;
    vector<vector<bool> > mapa(60,vector<bool>(60,false));
    vector<vector<Pos> > m(60, vector<Pos>(60,Pos(-1,-1))); 
    
    Pos fi=Pos(-1,-1);
    Pos seg;
    seg.i=posi.i;
    seg.j=posi.j; 
    Pos actual;
    
    //cerr <<"principi "<< seg.i << " " <<seg.j <<  endl;
    
    mapa[posi.i][posi.j]=true; //posem la posi actual en visitat
    queue<Pos> cua;
    cua.push(posi);
    Pos aux;

    while(not cua.empty()){ //mentre no estigui buida, no hem trobat el k volem i/o tots elem visitats
        actual = cua.front();
        cua.pop();
        
        //cerr<<"aqui el -1? "<<endl;
        if(cell(actual.i,actual.j).type == Station) { //si hi ha una stacio
            //cerr<<"aqui no pot ser -1  "<<endl;
            while(not cua.empty())cua.pop();//buidem la cua per sortir del bucle
            fi.i=actual.i; //guardem la posi de la city
            fi.j=actual.j;
                        
        }    
        else{ //si encara no trobem la ciutat
            //cerr<<"aqui el -1? nose  "<<endl;
            for(int k=0; k<8; ++k){ //Mirem el voltant de la posicio si hi ha una pos amb prioritat devant les altres
                aux.i = actual.i + I[k];
                aux.j = actual.j + J[k];

                //cerr<<"aqui el -1? tu creus? "<<endl; //aqui tenim el problema del -1
                if(pos_ok(aux) and pot_moure_cgas(player,aux.i,aux.j) and not mapa[aux.i][aux.j]){
                   //cerr<<"aqui el -1? ho accepta "<<endl;
                        mapa[aux.i][aux.j]=true;
                        cua.push(aux); 
                        m[aux.i][aux.j].i = actual.i;//afegim la posicio a la cua, en el cas que hi hagues una city la cua estarà buida i afegirem nomes aquesta posicio(prioritaria)
                        m[aux.i][aux.j].j = actual.j;
                        
                    }
            }  
        }
    }
    stack<Pos> s;
    if(fi != Pos(-1,-1)){ //no entraaaaaaaaaa
        //cerr << "if fi" << endl;
        // cerr << fi.i << " " << fi.j << " fi " <<endl;
        //cerr << posi.i << " " << posi.j << "posi " <<endl;
        if(fi==posi)s.push(fi); //linia perque sino si coincidia, al guardar a seg el s.top() feia un segmentation xk no hi havia res a s
        while(fi != posi){
            //cerr<<"if fi!=posi"<<endl;
            int ax;  
            s.push(fi);
            ax = m[fi.i][fi.j].i;
            fi.j = m[fi.i][fi.j].j;
            fi.i=ax;
        }
        //cerr<< "top.i "<<s.top().i<<" top.j "<< s.top().j<<endl;
        //cerr<<"o fi==posi o fora bucle"<<endl;
        seg.i = s.top().i;
        seg.j = s.top().j;
        //cerr<< "seg.i "<<seg.i<<" seg.j "<< seg.j<<endl;
        
        
    }
    //cerr << seg.i << " " <<seg.j <<  endl;
    for(int k =0 ; k < 8 ; ++k){ //com ja hem trobat la posició a la que volem mourens, busquem quina direccio es        
        Pos p(posi+ Dir(k));
        int pi=p.i;
        int pj=p.j;
        if(pi== seg.i and pj == seg.j){
            command(id,Dir(k));
            
           //cerr << p.i << " " << p.j << " k= "<<k<<  endl;
           k=8;
        }
    }
    
    
}
void BFS_water(int id){
    Pos posi=unit(id).pos;
    int player = unit(id).player;
    vector<vector<bool> > mapa(60,vector<bool>(60,false));
    vector<vector<Pos> > m(60, vector<Pos>(60,Pos(-1,-1))); 
    
    Pos fi=Pos(-1,-1);
    Pos seg;
    seg.i=posi.i;
    seg.j=posi.j; 
    Pos actual;
    
    //cerr <<"principi "<< seg.i << " " <<seg.j <<  endl;
    
    mapa[posi.i][posi.j]=true; //posem la posi actual en visitat
    queue<Pos> cua;
    cua.push(posi);
    Pos aux;

    while(not cua.empty()){ //mentre no estigui buida, no hem trobat el k volem i/o tots elem visitats
        actual = cua.front();
        cua.pop();
        
        //cerr<<"aqui el -1? "<<endl;
        if(es_adjacent_aigua(actual)) { //si hi ha una aigua adjacent
            //cerr<<"aqui no pot ser -1  "<<endl;
            while(not cua.empty())cua.pop();//buidem la cua per sortir del bucle
            fi.i=actual.i; //guardem la posi de la city
            fi.j=actual.j;
                        
        }    
        else{ //si encara no trobem la ciutat
            //cerr<<"aqui el -1? nose  "<<endl;
            for(int k=0; k<8; ++k){ //Mirem el voltant de la posicio si hi ha una pos amb prioritat devant les altres
                aux.i = actual.i + I[k];
                aux.j = actual.j + J[k];

                //cerr<<"aqui el -1? tu creus? "<<endl; //aqui tenim el problema del -1
                if(pos_ok(aux) and pot_moure_w(player,aux.i,aux.j) and not mapa[aux.i][aux.j]){
                   //cerr<<"aqui el -1? ho accepta "<<endl;
                        mapa[aux.i][aux.j]=true;
                        cua.push(aux); 
                        m[aux.i][aux.j].i = actual.i;//afegim la posicio a la cua, en el cas que hi hagues una city la cua estarà buida i afegirem nomes aquesta posicio(prioritaria)
                        m[aux.i][aux.j].j = actual.j;
                        
                    }
            }  
        }
    }
    stack<Pos> s;
    if(fi != Pos(-1,-1)){ //no entraaaaaaaaaa
        //cerr << "if fi" << endl;
        // cerr << fi.i << " " << fi.j << " fi " <<endl;
        //cerr << posi.i << " " << posi.j << "posi " <<endl;
        if(fi==posi)s.push(fi); //linia perque sino si coincidia, al guardar a seg el s.top() feia un segmentation xk no hi havia res a s
        while(fi != posi){
            //cerr<<"if fi!=posi"<<endl;
            int ax;  
            s.push(fi);
            ax = m[fi.i][fi.j].i;
            fi.j = m[fi.i][fi.j].j;
            fi.i=ax;
        }
        //cerr<< "top.i "<<s.top().i<<" top.j "<< s.top().j<<endl;
        //cerr<<"o fi==posi o fora bucle"<<endl;
        seg.i = s.top().i;
        seg.j = s.top().j;
        //cerr<< "seg.i "<<seg.i<<" seg.j "<< seg.j<<endl;
        
        
    }
    //cerr << seg.i << " " <<seg.j <<  endl;
    for(int k =0 ; k < 8 ; ++k){ //com ja hem trobat la posició a la que volem mourens, busquem quina direccio es        
        Pos p(posi+ Dir(k));
        int pi=p.i;
        int pj=p.j;
        if(pi== seg.i and pj == seg.j){
            command(id,Dir(k));
            
           //cerr << p.i << " " << p.j << " k= "<<k<<  endl;
           k=8;
        }
    }
}
void BFS_road(int id){
    Pos posi=unit(id).pos;
    int player = unit(id).player;
    vector<vector<bool> > mapa(60,vector<bool>(60,false));
    vector<vector<Pos> > m(60, vector<Pos>(60,Pos(-1,-1))); 
    
    Pos fi=Pos(-1,-1);
    Pos seg;
    seg.i=posi.i;
    seg.j=posi.j; 
    Pos actual;
    
    //cerr <<"principi "<< seg.i << " " <<seg.j <<  endl;
    
    mapa[posi.i][posi.j]=true; //posem la posi actual en visitat
    queue<Pos> cua;
    cua.push(posi);
    Pos aux;

    while(not cua.empty()){ //mentre no estigui buida, no hem trobat el k volem i/o tots elem visitats
        actual = cua.front();
        cua.pop();
        
        //cerr<<"aqui el -1? "<<endl;
        if(cell(actual.i,actual.j).type == Road) { //si hi ha una road
            //cerr<<"aqui no pot ser -1  "<<endl;
            while(not cua.empty())cua.pop();//buidem la cua per sortir del bucle
            fi.i=actual.i; //guardem la posi de la city
            fi.j=actual.j;
                        
        }    
        else{ //si encara no trobem la ciutat
            //cerr<<"aqui el -1? nose  "<<endl;
            for(int k=0; k<8; ++k){ //Mirem el voltant de la posicio si hi ha una pos amb prioritat devant les altres
                aux.i = actual.i + I[k];
                aux.j = actual.j + J[k];

                //cerr<<"aqui el -1? tu creus? "<<endl; //aqui tenim el problema del -1
                if(pos_ok(aux) and pot_moure_croad(player,aux.i,aux.j) and not mapa[aux.i][aux.j]){
                   //cerr<<"aqui el -1? ho accepta "<<endl;
                        mapa[aux.i][aux.j]=true;
                        cua.push(aux); 
                        m[aux.i][aux.j].i = actual.i;//afegim la posicio a la cua, en el cas que hi hagues una city la cua estarà buida i afegirem nomes aquesta posicio(prioritaria)
                        m[aux.i][aux.j].j = actual.j;
                        
                    }
            }  
        }
    }
    stack<Pos> s;
    if(fi != Pos(-1,-1)){ //no entraaaaaaaaaa
        //cerr << "if fi" << endl;
        // cerr << fi.i << " " << fi.j << " fi " <<endl;
        //cerr << posi.i << " " << posi.j << "posi " <<endl;
        if(fi==posi)s.push(fi); //linia perque sino si coincidia, al guardar a seg el s.top() feia un segmentation xk no hi havia res a s
        while(fi != posi){
            //cerr<<"if fi!=posi"<<endl;
            int ax;  
            s.push(fi);
            ax = m[fi.i][fi.j].i;
            fi.j = m[fi.i][fi.j].j;
            fi.i=ax;
        }
        //cerr<< "top.i "<<s.top().i<<" top.j "<< s.top().j<<endl;
        //cerr<<"o fi==posi o fora bucle"<<endl;
        seg.i = s.top().i;
        seg.j = s.top().j;
        //cerr<< "seg.i "<<seg.i<<" seg.j "<< seg.j<<endl;
        
        
    }
    //cerr << seg.i << " " <<seg.j <<  endl;
    for(int k =0 ; k < 8 ; ++k){ //com ja hem trobat la posició a la que volem mourens, busquem quina direccio es        
        Pos p(posi+ Dir(k));
        int pi=p.i;
        int pj=p.j;
        if(pi== seg.i and pj == seg.j){
            command(id,Dir(k));
            
           //cerr << p.i << " " << p.j << " k= "<<k<<  endl;
           k=8;
        }
    }
    
    
}
  void move_warriors() {
    //if (round()% 4 != me()) return; // Si no es congruent mod4 amb mi, no ens movem xk ho diu l'enunciat(els warriors)
  
    VE W = warriors(me());
    int n = W.size();
    for (int i = 0; i < n; ++i) {
      int id = W[i];
      if(no_ataca_w(id)){ //cerr<<"el id = "<<id<<"fara bfs "<<endl;
          
        if(unit(id).water<15)BFS_water(id);
        BFS_warrior(id);
        
    }
      //cerr<<"el id = "<<id<<"ha fet bfs "<<endl;
        //if(unit(id).water<15)BFS_water(id);
      
    }
  }

  void move_cars() {
    VE C = cars(me());
    int n= C.size();
    vector < Pos > pc(n);
    for(int aux=0;aux<n;++aux){
        int id=C[aux];
        pc[aux]=unit(id).pos;
    }
    
    for (int k=0; k<n;++k) {
        int id=C[k];
        
        if(no_ataca_c(id)){
            if(unit(id).food<25)BFS_gas(id);
            if(cell(unit(id).pos).type == Desert)BFS_road(id);//ALERT BUSCA CARRETERA
            BFS_car(id,pc,k);
        }
      }
   }
  


  /*
   * Play method, invoked once per each round.
   */
  void play () {
    move_warriors();
    move_cars();
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
