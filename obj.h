#pragma once
int debug_num;
struct Object;
struct BaseModelObj{
    unsigned int VAO=-1;
    Shader shader;
    std::vector<Object*> objlist;
    int category = 0; // 模型种类
    string modelPath;
    BaseModelObj(const std::string Shapath1,const std::string Shapath2,
        const std::string MPath):shader(Shapath1,Shapath2),modelPath(MPath){}

    BaseModelObj(const std::string Shapath1, const std::string Shapath2) :shader(Shapath1, Shapath2){}

    void setModelPath(const string path) {
        modelPath = path;
    }

    virtual void render(glm::vec3 &lightPos,glm::mat4 &projection,glm::mat4 &view,glm::vec3 &viewPos){
        std::cout<<"calling a virtual function \"BaseModeObj::render\".\n";
        while(1);
    };
};

struct Object{
    glm::mat4 modelmat;
    glm::mat4 scalemat;
    BaseModelObj* Mod;
    Object(
        glm::vec3 offset=glm::vec3(0,0,0),
        BaseModelObj* M = nullptr
        ):
            Mod(M),
            modelmat(glm::translate(glm::mat4(1.0f),offset)),
            scalemat(glm::mat4(1.0f))
    {
        if(M!=nullptr) bindshadermodel(M);
    }
    /*Object(const Object & other):Mod(other.Mod),modelmat(other.modelmat),scalemat(other.scalemat){
        if(Mod!=nullptr) bindshadermodel(Mod);
    }*/
    void bindshadermodel(BaseModelObj * M){
        Mod = M;
        std::vector<Object*> & mod = Mod->objlist;
        int l=0,r=mod.size(),mid;
        while (l < r) {
            mid = (l+r)/2;
            if (mod[mid] < this) l = mid + 1;
            else if (mod[mid] > this) r = mid;
            else break;
        }
        mod.insert(mod.begin()+l, this);
    }
    void delshadermodel(){
        std::vector<Object*> & mod = Mod->objlist;
        int l=0,r=mod.size(),mid=0,num=0;
        while (l < r){
            ++num;
            mid = (l + r) / 2;
            if (mod[mid] == this) {
                l = mid; break;
            }
            else if (mod[mid] < this) l = mid+1;
            else if (mod[mid] > this) r = mid;
            if(num>10000){
                std::cout<<"endless loop in object.\n";
            }
        }
        if(mod[l] == this){
            mod.erase(mod.begin()+l);
        }else {
            std::cout<<"warning: " << l << " " << r << " " << mod[l] << " " << this << '\n';
        }
        debug_num = num;
        Mod = nullptr;
    }

    glm::vec3 getPos() const{
        return glm::vec3(modelmat[3][0],modelmat[3][1],modelmat[3][2]);
    }
    void setPos(double x,double y,double z){
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        modelmat = translationMatrix * glm::mat4(glm::mat3(modelmat));
        //modelmat = glm::translate(modelmat,glm::vec3(x-modelmat[3][0],y-modelmat[3][1],z-modelmat[3][2]));
    }

    void rotate(double x, double y) {
        
    }

    void shift(double x,double y,double z){
        modelmat = glm::translate(modelmat,glm::vec3(x,y,z));
    }
    glm::mat4 getmodel()const{
        return modelmat * scalemat;
    }
    ~Object(){
        if(Mod!=nullptr) delshadermodel();
    }
};