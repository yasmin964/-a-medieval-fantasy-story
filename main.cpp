#include <fstream> // For file I/O
#include <sstream> // For string stream
#include <vector>
#include <map>
#include <algorithm>
#include <iostream> // For standard I/O
#include <iomanip> // For setting precision
#include <memory> // For smart pointers
#include <stdexcept> // For exceptions
using namespace std;

class PhysicalItem;
class Character;
class Spell;

template<typename T>
concept derived = is_base_of<PhysicalItem, T>::value;


//ostream& operator<<(std::ostream& os, const PhysicalItem& obj) {
//    obj.print(os);
//    return os;
//};
ostream& operator<<(std::ostream& os, const Character& obj) {
    // Implement the output functionality for MyClass objects
    return os;
};
//class for all characters, the main characteristics and methods
class Character{
private:
    int healthPoints;
    string name;
    vector<PhysicalItem*> items;
    //    vector<Spell*> spellBook;
public:
    Character(const string &charName, int hp) : name(charName), healthPoints(hp){}

    virtual ~Character() {}
    void takeDamage(int damage) {
        healthPoints -= damage;
        if (healthPoints <= 0) {
            die();
        }
    }
    void heal(int healValue){
        healthPoints +=healValue;
    }
    bool die() {
        cout << name << " has died..." << endl;
        items.clear();
        return true;
    }
    string getName() const{
        return name;
    }
    int getHP() const {
        return healthPoints;
    }
    void remove(PhysicalItem* item) {
        auto it = find(items.begin(), items.end(), item);
        if (it != items.end()) {
            items.erase(it);
        }
    }
    bool isValid(){
        if (healthPoints <= 0) {//это проверка на то что он не умер,
            //нужно ли проверять был ли он до этого, вообще был ли он и как
            return false;
        }
        return true;
    }
    std::map<std::string, Character*> charactersMap;

    Character* findCharacterByName(const std::string& name) {
        auto it = charactersMap.find(name);
        if (it != charactersMap.end()) {
            return it->second;
        }
        return nullptr; // Character not found
    }
//    void addSpellSheet(Spell* sheet) {
//        spellBook.push_back(sheet);
//    }


    friend class PhysicalItem;

    template <typename T>
    friend ostream& operator<<(std::ostream& os, const T& obj);
     virtual void obtainItemSideEffect(PhysicalItem *item, shared_ptr<Character> user){} ;
     virtual void loseItemSideEffect(PhysicalItem *item, shared_ptr<Character> user){};
     virtual void print (shared_ptr<Character> user){};

};
bool operator==(const Character& a, const Character& b) {
    return a.getName() == b.getName() && a.getHP() == b.getHP();
}
//bool operator!=(const Character& a, const Character& b) {
//    return !(a==b);
//}

//class for all item, the main characteristics and methods
class PhysicalItem{
private:
    bool isUsableOnce;
    string name;
    Character *owner;
public:
    PhysicalItem(const string &name, Character *owner, bool isUsableOnce) : name(name), owner(owner),
                                                                            isUsableOnce(isUsableOnce) {}
    virtual ~PhysicalItem() {}

    void use (Character user, Character target ){}
    string getName()const{
        return name;
    }
    virtual void setup(){};
    virtual void useLogic(shared_ptr<Character> user, shared_ptr<Character> target){};
    Character *getOwner()const{
        return owner;
    }
protected:
//    void useCondition(Character* user, Character* target){
//        // Реализация условия использования предмета на целевом персонаже
//        // проверяем, может ли пользователь использовать предмет на цели
//
//        if (user != owner) {
//            cout << "Error caught"<<endl;
//            return;
//        }
//        if(target->getHP()<=0){
//            cout << "Error caught"<<endl;
//            return;
//        }
//    }
    void giveDamageTo(Character to, int damage)const{
        to.takeDamage(damage);
    }
    void giveHealTo(Character to, int heal){
        to.heal(heal);
    }

    //deals with removing items form their owner
    virtual void afterUse(shared_ptr<Character> user){}
    virtual void print(PhysicalItem item)const {};

    friend ostream& operator<<(std::ostream& os, const PhysicalItem& obj);

};
ostream& operator<<(std::ostream& os, const PhysicalItem& obj) {
    obj.print(obj);
    return os;
};
class Weapon: public PhysicalItem{
private:
    int damage;

public:
    void print( Weapon& weapon)const {
        cout<<weapon.getName()<<":"<<weapon.getDamage()<<endl;
    }
    Weapon(int damage, const string &name, Character *owner) : damage(damage), PhysicalItem(name, owner, false) {}
    ~Weapon(){};
    int getDamage()const{
        return damage;
    };
    void setup(){}
    //the main logic of weapon it takes damage on the character
    void useLogic(shared_ptr<Character> user, shared_ptr<Character> target) override{
        target->takeDamage(damage);
    }
};
class Potion : public PhysicalItem{
private:
    int healValue;
    bool used;
protected:
    void print(Potion potion)const{
        cout<<potion.getName()<<":"<<potion.getHealValue()<<endl;
    }
public:
    Potion(int healValue, const string& name, Character* owner) : healValue(healValue), PhysicalItem(name, owner, true), used(false) {}
    int getHealValue()const{
        return healValue;
    }
    //using of potion-> it helps to increase the health value
    void useLogic(shared_ptr<Character> user, shared_ptr<Character> target){
        if(used) {
            cout<<"Error caught"<<endl;
        }
        target->heal(healValue);
        used = true;
        afterUse(user);
    }
    void setup(){}
    void afterUse(shared_ptr<Character> user) override {
        user->remove(this);
    };
};
class Spell : public PhysicalItem {
private:
    vector<shared_ptr<Character>> allowedTargets;
    bool used;
public:
    Spell(const string& name, Character* owner)
            : PhysicalItem(name, owner, true), used(false) {}

    void addAllowedTarget(shared_ptr<Character> target) {
        if (target) {
            allowedTargets.push_back(target);
        }
    }
//the main logic of spell using describes here
    void useLogic(shared_ptr<Character> user, shared_ptr<Character> target) override {
        if (!target->isValid() || used) {
            cout << "Error caught" << endl;
            return;
        }

        auto it = find_if(allowedTargets.begin(), allowedTargets.end(),
                          [&](const shared_ptr<Character> &t) { return t == target; });
        if (it != allowedTargets.end()) {
            cout << user->getName() << " casts " << getName() << " on " << target->getName() << "!" << endl;
            target->die();
            target->loseItemSideEffect(this, user);
            used = true;

        } else {
            cout << "Error caught" << endl;
        }
        afterUse(user);
    }
    //after use spell the character is die
    void afterUse(shared_ptr<Character> user) override {
        user->remove(this);

    };
    bool isUsed() const {
        return used;
    }
    int getNumAllowedTargets()const{
//        for(auto el: allowedTargets) {
//            cout << *el << "\n";
//        }
//        cout << "\n";
        //TODO delete check
        return allowedTargets.size();
    }
    void setup(shared_ptr<Character> target) {
        if (target) {
//            cout << target->getName() << "\n";
            auto it = find(allowedTargets.begin(), allowedTargets.end(), target);
            if (it == allowedTargets.end()) {
                allowedTargets.push_back(target);
            }
        } else {
            cout << "Error caught3\n";
        }
    }
};

template <typename T>//continue template Container
class Container {
protected:
    vector<T> elements;
public:
    Container() {}

    ~Container() {}
    bool find(const T& item) {
        return std::find(elements.begin(), elements.end(), item) != elements.end();
    }
    void removeItem(T newItem){
        elements.erase(std::find(elements.begin(), elements.end(), newItem));
    }
    void addItem(T newItem){
        elements.emplace(std::find(elements.begin(), elements.end(), newItem), newItem);
    }
};

template <derived T>
class Container<T>{
public:
    map<string , T> elements;
    // Constructor
    Container(){}
    ~Container() {}
    void additem(const T& newItem) {
        elements.emplace(newItem.getName(), newItem);
    }
    void removeItem(T newItem){
        elements.erase(newItem.getName());
    }
    bool find(T item){
        if(elements.find(item.getName())){
            return true;
        }
        return false;
    }
    T find(string itemName){
        if(elements.contains(itemName)){
            return itemName;
        }
    }
    void removeItem(string itemName){
        auto it = elements.find(itemName);
        if (it != elements.end()) {
            elements.erase(it);
        }
    }
    int size(){
        return elements.size();
    }
    map<string, T> getElements() const {
        return elements;
    }
};
template <derived T>
class ContainerWithMaxCapacity:public Container<T>{
private:
    int maxCapacity;
public:
    ContainerWithMaxCapacity(int maxCapacity) : Container<T>(), maxCapacity(maxCapacity) {}

    void addItem(const T& item) {
        if (this->elements.size() < maxCapacity) {
            this->elements.push_back(item);
        }
    }

    void show(){
        for (const auto& element : this->elements) {
            cout << element.first << endl;
        }
    }
};
using Arsenal = ContainerWithMaxCapacity<Weapon>;
using MedicalBag = ContainerWithMaxCapacity<Potion>;
using SpellBook = ContainerWithMaxCapacity<Spell>;

class WeaponUser: virtual public Character{
public:
    Arsenal arsenal;
    string usName;
    string tarName;

    WeaponUser(int maxCapacity, const string &characterName, int hp) : arsenal(maxCapacity), Character(characterName, hp) {}

    //what happens when character attacks another one
    void attack(shared_ptr<Character> owner, shared_ptr<Character> target, string weaponName ){
        if (!owner || !target) {
            cout << "Error caught" << endl;
            return;
        }
        if (!owner->isValid() || !target->isValid()) {
            cout << "Error caught" << endl;
            return;
        }
        bool found = false;
        // Find the weapon in the arsenal
        for (auto& element : this->arsenal.elements) {
            Weapon& weapon = element.second;
            if (weapon.getName() == weaponName) {
                // Use the weapon's useLogic method to perform the attack
                cout<<owner->getName()<<" attacks "<<target->getName()<<" with their "<<weaponName<<"!"<<endl;
                weapon.useLogic(owner, target);
                found = true;
                return; // Exit the loop after the attack
            }
        }
        if(!found){
            // If weapon is not found of certain character
            cout << "Error caught" << endl;
        }
    }
    // show all arsenal
    void showWeapons(const map<string, Weapon>& weapons) {
        if (weapons.empty()) {
            cout << endl;
            return;
        } else  {
            for (const auto& [name, weapon] : weapons) {
                cout << weapon.getName() << ":" << weapon.getDamage() << " ";
            }
            cout<<endl;
        }
    }
};
class PotionUser: virtual public Character{
public:
    PotionUser(int maxCapacity, const string &characterName, int hp) : medicalBag(maxCapacity), Character(characterName, hp) {}
    ~PotionUser(){}

    //what happens when character drink the potion from another one
    void drink(shared_ptr<Character> owner, shared_ptr<Character> target, string potionName){
        if (!owner || !target) {
            cout << "Error caught" << endl;
            return;
        }
        if (!owner->isValid() || !target->isValid()) {
            cout << "Error caught" << endl;
            return;
        }

        // Find the weapon in the arsenal
        for (auto& element : this->medicalBag.elements) {
            Potion& potion = element.second;
            if (potion.getName() == potionName) {
                // Use the weapon's useLogic method to perform the attack
                cout<<target->getName()<<" drinks "<<potion.getName()<<" from "<<owner->getName()<<"."<<endl;
                potion.useLogic(owner, target);
                return;
            }
        }
        cout << "Error caught" << endl;
        return;
    }
    //show all medicalBag of certain character
    void showPotions(const map<string, Potion>& potions) {
        if (potions.empty()) {
            cout << endl;
            return;
        }
        else{
            for (const auto& [name, potion] : potions) {
                if(potion.getHealValue()>0){
                    cout << potion.getName() << ":" << potion.getHealValue() << " ";
                }
            }
            cout<<endl;
        }
    }

    MedicalBag medicalBag;
};
class SpellUser: virtual public Character{
public:
    SpellBook spellBook;
public:
    SpellUser(int maxCapacity, const string &characterName, int hp) : spellBook(maxCapacity), Character(characterName, hp) {}
    ~SpellUser(){}
    //what happens when character cast the spell from another one
    void cast(shared_ptr<Character> owner, shared_ptr<Character> target, string spellName){
        if (!owner || !target) {
            cout << "Error caught" << endl;
            return;
        }
        if (!owner->isValid() || !target->isValid()) {
            cout << "Error caught" << endl;
            return;
        }
        // Find the weapon in the arsenal
        for (auto& element : this->spellBook.elements) {
            Spell& spell = element.second;
            if (spell.getName() == spellName) {
                // Use the weapon's useLogic method to perform the attack
                spell.useLogic(owner, target);
                return;
            }
        }
        cout << "Error caught" << endl;

    }
    //show all spell book of certain character
    void showSpells(const map<string, Spell>& spells) {
        if (spells.empty()) {
            cout << endl;
            return;
        }
        for (const auto& [name, spell] : spells) {
            if (!spell.isUsed()) { // Добавлено условие проверки, использовано ли заклинание
                cout << spell.getName() << ":" << spell.getNumAllowedTargets() << " ";
            }
        }
        cout << endl;
    }
    void addSpell(Spell spell) {
        spellBook.additem(spell);
    }
};
class Fighter:public WeaponUser, public PotionUser {
public:
    static int maxAllowedWeapons;
    static int maxAllowedPotions;
    std::string charName;
    int healthPoints;
    void print(shared_ptr<Character> user )override{
        cout<<"A new fighter came to town, "<<user->getName()<<"."<< endl;
    }
    // show the fighter character
    void printR(shared_ptr<Character> user){
        if (user) {
            cout << user->getName() << ":" << "fighter" << ":" << user->getHP() << " ";
        } else {
            cout << "Error caught" << endl;
        }
    }

    Fighter(const string &charName, int hp) : WeaponUser(3, charName,hp), PotionUser(5, charName, hp), Character(charName, hp) {}

    std::string getName() const {
        return charName;
    }
    //the main logic when fighter obtain an item
    void obtainItemSideEffect(PhysicalItem *item, shared_ptr<Character> user) override {
        if (!user->isValid()) {
            cout << "Error caught" << endl;
            return;
        }
        if (arsenal.size() >= maxAllowedWeapons) {
            cout << "Error caught" << endl;
            return;
        }
        if (medicalBag.size() >= maxAllowedPotions) { // Изменено с > на >=
            cout << "Error caught" << endl;
            return; // Добавлено возвращение из функции, чтобы предотвратить добавление нового зелья
        }
        Weapon *weapon = dynamic_cast<Weapon *>(item);
        if (weapon) {
            arsenal.additem(*weapon);
            cout << user->getName() << " just obtained a new weapon called " << weapon->getName() <<"."<< endl;
        }
        Potion *potion = dynamic_cast<Potion *>(item);
        if (potion) {
            medicalBag.additem(*potion);
            cout << user->getName() << " just obtained a new potion called " << potion->getName() <<"."<< endl;
        }
//        else {
//            // Item is not a weapon
//            os << "Error caught" << endl;
//        }
    }

private:
    //the main logic when fighter lose an item
    void loseItemSideEffect(PhysicalItem *item, shared_ptr<Character> user) override{
        if (user->die()) {
            if (Weapon *weapon = dynamic_cast<Weapon *>(item)){
                arsenal.removeItem(weapon->getName());
            }else if (Potion* potion = dynamic_cast<Potion*>(item)){
                medicalBag.removeItem(potion->getName());
            }else {
                // Item is not a weapon or potion
                cout << "Error caught" << endl;
            }
        }
    }
};
int Fighter::maxAllowedWeapons = 3;
int Fighter::maxAllowedPotions = 5 ;
class Archer:public WeaponUser, public PotionUser, public SpellUser {
public:
    static int maxAllowedWeapons;
    static int maxAllowedPotions;
    static int maxAllowedSpells;
    std::string charName;
    int healthPoints;
    void print( shared_ptr<Character> user)override{
        cout<<"A new archer came to town, "<<user->getName()<<"."<< endl;
    }
    void printR(shared_ptr<Character> user){
        if (user) {
            cout << user->getName() << ":" << "archer" << ":" << user->getHP() << " ";
        } else {
            cout << "Error caught" << endl;
        }
    }

    Archer(const string &charName, int hp) : WeaponUser(2, charName, hp),
                                                  PotionUser(3, charName, hp), SpellUser(2, charName, hp), Character(charName, hp) {}


    std::string getName() const {
        return charName;
    }

    void obtainItemSideEffect(PhysicalItem *item, shared_ptr<Character> user) override {
        if (arsenal.size() >= maxAllowedWeapons) {
            cout << "Error caught" << endl;
            return;
        }
        if (medicalBag.size() >= maxAllowedPotions) {
            cout << "Error caught" << endl;
            return;
        }
        if (spellBook.size() >= maxAllowedSpells) {
            cout << "Error caught" << endl;
            return;
        }
        Weapon *weapon = dynamic_cast<Weapon *>(item);
        if (weapon) {
            arsenal.additem(*weapon);
            cout << user->getName() << " just obtained a new weapon called " << weapon->getName() <<"."<< endl;
        }
        Potion *potion = dynamic_cast<Potion *>(item);
        if (potion) {
            medicalBag.additem(*potion);
            cout << user->getName() << " just obtained a new potion called " << potion->getName() <<"."<< endl;
        }
        Spell *spell = dynamic_cast<Spell *>(item);
        if (spell) {
            spellBook.additem(*spell);
            cout << user->getName() << " just obtained a new spell called " << spell->getName() <<"."<< endl;
        }
    }

private:

    void loseItemSideEffect(PhysicalItem *item, shared_ptr<Character> user) override{
        if (user->die()) {
            if (Weapon *weapon = dynamic_cast<Weapon *>(item)){
                arsenal.removeItem(weapon->getName());
            }else if (Potion* potion = dynamic_cast<Potion*>(item)){
                medicalBag.removeItem(potion->getName());
            }else if (Spell *spell = dynamic_cast<Spell *>(item)){
                spellBook.removeItem(spell->getName());
            }
        }
    }
};

int Archer::maxAllowedPotions = 3;
int Archer::maxAllowedWeapons = 2 ;
int Archer::maxAllowedSpells = 2;
class Wizard:public SpellUser, public PotionUser {
public:
    static int maxAllowedSpells;
    static int maxAllowedPotions;
    std::string charName;
    int healthPoints;
    void print( shared_ptr<Character>  user) override{
        cout<<"A new wizard came to town, "<<user->getName()<<"."<< endl;
    }
    void printR(shared_ptr<Character> user){
        if (user) {
            cout << user->getName() << ":" << "wizard" << ":" << user->getHP() << " ";
        } else {
            cout << "Error caught" << endl;
        }
    }

    Wizard(const string &charName, int hp) : PotionUser(10, charName, hp), SpellUser(10, charName, hp), Character(charName, hp) {}

    std::string getName() const {
        return charName;
    }

    void obtainItemSideEffect(PhysicalItem *item, shared_ptr<Character> user) override {
        if (spellBook.size() >= maxAllowedSpells) {
            cout << "Error caught" << endl;
            return;
        }
        if (medicalBag.size() >= maxAllowedPotions) {
            cout << "Error caught" << endl;
            return;
        }
        Spell *spell = dynamic_cast<Spell *>(item);
        if (spell) {
            spellBook.additem(*spell);
            cout << user->getName() << " just obtained a new spell called " << spell->getName() <<"."<< endl;
        }
        Potion *potion = dynamic_cast<Potion *>(item);
        if (potion) {
            medicalBag.additem(*potion);
            cout << user->getName() << " just obtained a new potion called " << potion->getName() <<"."<< endl;
        }
    }

    void loseItemSideEffect(PhysicalItem *item, shared_ptr<Character> user) override{
        if (user->die()) {
            if (Spell *spell = dynamic_cast<Spell *>(item)){
                spellBook.removeItem(spell->getName());
            }else if (Potion* potion = dynamic_cast<Potion*>(item)){
                medicalBag.removeItem(potion->getName());
            }else {
                // Item is not a weapon or potion
                cout << "Error caught" << endl;
            }
        }
    }
};
int Wizard::maxAllowedPotions = 10;
int Wizard::maxAllowedSpells = 10 ;
int main() {
    map<string, shared_ptr<Character>> characters;
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    int numLines;
    cin >> numLines;
    cin.ignore();
    for (int i = 0; i < numLines; ++i) {
        string line;
        getline(cin, line);
        istringstream iss(line);
        string eventType;
        iss >> eventType;

        if (eventType == "Create") {
            string objectType;
            iss >> objectType;
            if (objectType == "character") {
                string charType, charName;
                int initHP;
                iss >> charType >> charName >> initHP;
                if (charType == "fighter") {
                    auto fighter = make_shared<Fighter>(charName, initHP);
                    fighter->print(fighter);
                    characters.emplace(charName, fighter);

                } else if (charType == "wizard") {
                    auto wizard = make_shared<Wizard>(charName, initHP);
                    wizard->print( wizard);
                    characters.emplace(charName, wizard);

                } else if (charType == "archer") {
                    auto archer = make_shared<Archer>(charName, initHP);
                    archer->print( archer);
                    characters.emplace(charName, archer);

                }
            }
            if (objectType == "item") {
                iss >> objectType;
                if (objectType == "weapon") {
                    string itemType, itemName, charName;
                    int damage;
                    iss >> charName >> itemName >> damage;

                    // Проверка на существование персонажа
                    auto userIter = characters.find(charName);
                    if (userIter == characters.end()) {
                        cout << "Error caught" << endl;
                        continue;
                    }

                    if (damage <= 0) { // Check if the damage value is valid
                        cout << "Error caught" << endl;
                        continue;
                    }
                    auto user = characters[charName];
                    if (auto userF = dynamic_pointer_cast<Fighter>(user)) {

                        if (userF->isValid()) {
                            Weapon weapon(damage, itemName, userF.get());
                            userF->obtainItemSideEffect(&weapon, userF);
                            userF->arsenal.removeItem(weapon);
                        } else {
                            cout << "Error caught" << endl;
                        }
                    } else if (auto userA = dynamic_pointer_cast<Archer>(user)) {

                        if (userA->isValid()) {
                            Weapon weapon(damage, itemName, userA.get());
                            userA->obtainItemSideEffect(&weapon, userA);
                            userA->arsenal.removeItem(weapon);
                        } else {
                            cout << "Error caught" << endl;
                        }
                    } else {
                        cout << "Error caught" << endl; // Не удалось определить тип персонажа
                    }

                } else if (objectType == "potion") {
                    string itemName, charName;
                    int healValue;
                    iss >> charName >> itemName >> healValue;

                    // Проверка на существование персонажа
                    auto userIter = characters.find(charName);
                    if (userIter == characters.end()) {
                        cout << "Error caught" << endl;
                        continue;
                    }

                    if (healValue <= 0) {
                        cout << "Error caught" << endl;
                        continue;
                    }

                    auto user = characters[charName];
                    if (auto userF = dynamic_pointer_cast<Fighter>(user)) {

                        if (userF->isValid()) {
                            Potion potion(healValue, itemName, userF.get());
                            userF->obtainItemSideEffect(&potion, userF);
                            userF->medicalBag.removeItem(potion);
                        } else {
                            cout << "Error caught" << endl;
                        }
                    } else if (auto userA = dynamic_pointer_cast<Archer>(user)) {

                        if (userA->isValid()) {
                            Potion potion(healValue, itemName, userA.get());
                            userA->obtainItemSideEffect(&potion, userA);
                            userA->medicalBag.removeItem(potion);
                        } else {
                            cout << "Error caught" << endl;
                        }
                    }else if (auto userW = dynamic_pointer_cast<Wizard>(user)) {

                        if (userW->isValid()) {
                            Potion potion(healValue, itemName, userW.get());
                            userW->obtainItemSideEffect(&potion, userW);
                            userW->medicalBag.removeItem(potion);
                        } else {
                            cout << "Error caught" << endl;
                        }
                    } else {
                        cout << "Error caught" << endl; // Не удалось определить тип персонажа
                    }
                }
                    else if (objectType == "spell") {
                    string charName, itemName;
                    int m;
                    iss >> charName >> itemName >> m;

                    auto spellOwnerIter = characters.find(charName);
                    if (spellOwnerIter == characters.end()) {
                        cout << "Error caught" << endl;
                        continue;
                    }

                    auto spell = make_shared<Spell>(itemName, spellOwnerIter->second.get());
                    auto user = dynamic_pointer_cast<SpellUser>(spellOwnerIter->second);

                    for (int i = 0; i < m; ++i) {
                        string targetName;
                        iss >> targetName;
                        auto targetIter = characters.find(targetName);
                        if (targetIter != characters.end()) {
                            spell->addAllowedTarget(targetIter->second);

                        } else {
                            cout << "Error caught\n";
                        }
                    }

                    if (auto spellUser = dynamic_pointer_cast<SpellUser>(spellOwnerIter->second)) {
                        spellUser->addSpell(*spell);

                    } else {
                        cout << "Error caught\n";
                    }

//
//                    // Добавить заклинание персонажу
//                    user->addSpell(*spell);
                    // Вызвать побочный эффект получения заклинания
//                    user->obtainItemSideEffect(spell.get(), spellOwnerIter->second);
                }

            }
        }
            else if (eventType == "Attack") {
                string userName, targetName, weaponName;
                iss >> userName >> targetName >> weaponName;

                auto userIter = characters.find(userName);
                auto targetIter = characters.find(targetName);

            if (userIter != characters.end()) {
                if (targetIter != characters.end()) { // Check if target is found
                    auto user = dynamic_pointer_cast<WeaponUser>(userIter->second);
                    auto target = targetIter->second;
                    if (user) { // Check if user is not null
                        user->attack(userIter->second, target, weaponName);
                    } else {
                        cout << "Error caught" << endl;
                        continue;
                    }
                } else {
                    cout << "Error caught" << endl;
                    continue;
                }
            } else {
                cout << "Error caught" << endl;
                continue;
            }
        } else if (eventType == "Cast") {
                string userName, targetName, spellName;
                iss >> userName >> targetName >> spellName;

                auto userIter = characters.find(userName);
                auto targetIter = characters.find(targetName);

                //  user and target exist and they are WeaponUser and Character
                if (userIter != characters.end() ) {
                    if (targetIter != characters.end()) {
                        auto user = dynamic_pointer_cast<SpellUser>(userIter->second);
                        auto target = targetIter->second;
                        if (user) {
                            user->cast(user, target, spellName);
                        } else {
                            //is not a WeaponUser
                            cout << "Error caught" << endl;
                            continue;
                        }
                    } else {
                        // the user or the target does not exist
                        cout << "Error caught" << endl;
                        continue;
                    }
                }else {
                    // the user or the target does not exist
                    cout << "Error caught" << endl;
                    continue;
                }
            } else if (eventType == "Drink") {
            string userName, targetName, potionName;
            iss >> userName >> targetName >> potionName;

            auto userIter = characters.find(userName);
            auto targetIter = characters.find(targetName);

            //  user and target exist and they are WeaponUser and Character
            if (userIter != characters.end()) {
                if (targetIter != characters.end()) {
                    auto user = dynamic_pointer_cast<PotionUser>(userIter->second);
                    auto target = targetIter->second;
                    if (user) {
                        user->drink(user, target, potionName);
                    } else {
                        //is not a WeaponUser
                        cout << "Error caught" << endl;
                        continue;
                    }
                } else {
                    // the user or the target does not exist
                    cout << "Error caught" << endl;
                    continue;
                }
            } else {
                // the user or the target does not exist
                cout << "Error caught" << endl;
                continue;
            }
        }
            else if (eventType == "Dialogue") {
                string speaker, speech;
                int m;
                iss >> speaker >> m;

                if (speaker == "Narrator") {
                    for (int i = 0; i < m; i++) {
                        string word;
                        iss >> word;
                        speech += " " + word;
                    }
                    cout << "Narrator:" << speech << endl;
                } else {
                    cout << "Error caught" << endl;
                    continue;
                }

        } else if (eventType == "Show") {
            string objectType;
            iss >> objectType;
            if (objectType == "characters") {
                    for (const auto &pair: characters) {
                        auto character = pair.second;
                        if (character && character->getHP() > 0) {
                            auto userF = dynamic_pointer_cast<Fighter>(character);
                            auto userA = dynamic_pointer_cast<Archer>(character);
                            auto userW = dynamic_pointer_cast<Wizard>(character);
                            if (userF || userA || userW) {
                                if (userW) {
                                    userW->printR(userW); // Assuming outputFile is a stream where you want to print
                                }
                                if (userA) {
                                    userA->printR(userA);
                                }
                                if (userF) {
                                    userF->printR(userF);
                                }
                            }
                        }
                }
                cout << endl;
            }

            if (objectType == "weapons") {
                string charName;
                iss >> charName;
                auto user = characters[charName];
                auto userW = dynamic_pointer_cast<WeaponUser>(user);

                if (userW ) {
                    userW->showWeapons(userW->arsenal.elements);
                } else {
                    // Not a WeaponUser
                    cout << "Error caught" << endl;
                    continue;
                }
            } if (objectType == "potions") {
                string charName;
                iss >> charName;
                auto user = characters[charName];
                auto userP = dynamic_pointer_cast<PotionUser>(characters[charName]);
                if (userP) {
                    userP->showPotions(userP->medicalBag.elements);
                } else {
                    // Not a PotionUser
                    cout << "Error caught" << endl;
                    continue;
                }
            } else if (objectType == "spells") {
                string charName;
                iss >> charName;


                auto userS = dynamic_pointer_cast<SpellUser>(characters[charName]);
                if (userS) {
                    userS->showSpells(userS->spellBook.elements);
                } else {
                    // Not a SpellUser
                    cout << "Error caught" << endl;
                    continue;
                }
            }
            } else {
//                // Invalid event type
//                outputFile << "Error caught1" << endl;
//                continue;
            }
        }


        return 0;
    }