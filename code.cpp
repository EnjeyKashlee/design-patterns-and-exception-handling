#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

// --- Product Class ---
class Product
{
private:
    string id;
    string name;
    double price;

public:
    Product() {}
    Product(string id, string name, double price)
        : id(id), name(name), price(price) {}

    string getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
};

// --- CartItem Class ---
class CartItem
{
private:
    Product product;
    int quantity;

public:
    CartItem() {}
    CartItem(Product product, int quantity)
        : product(product), quantity(quantity) {}

    Product getProduct() const { return product; }
    int getQuantity() const { return quantity; }

    void incrementQuantity() { quantity++; }
};

// --- Payment Strategy Pattern ---
class PaymentStrategy
{
public:
    virtual string pay(double amount) = 0;
    virtual ~PaymentStrategy() {}
};

class CashPayment : public PaymentStrategy
{
public:
    string pay(double amount) override
    {
        cout << "\nPaying " << amount << " using Cash." << endl;
        return "Cash";
    }
};

class CardPayment : public PaymentStrategy
{
public:
    string pay(double amount) override
    {
        cout << "\nPaying " << amount << " using Credit/Debit Card." << endl;
        return "Credit/Debit Card";
    }
};

class GCashPayment : public PaymentStrategy
{
public:
    string pay(double amount) override
    {
        cout << "\nPaying " << amount << " using GCash." << endl;
        return "GCash";
    }
};

class PaymentContext
{
private:
    PaymentStrategy *strategy;

public:
    PaymentContext() : strategy(nullptr) {}

    void setStrategy(PaymentStrategy *strategy)
    {
        this->strategy = strategy;
    }

    string pay(double amount)
    {
        return strategy->pay(amount);
    }
};

// --- Order Class ---
class Order
{
private:
    int orderId;
    CartItem items[20];
    int itemCount;
    string paymentMethod;

public:
    Order() {}
    Order(int orderId) : orderId(orderId), itemCount(0) {}

    void addItem(CartItem item)
    {
        if (itemCount < 20)
            items[itemCount] = item;
        itemCount++;
    }

    void setPaymentMethod(string method)
    {
        paymentMethod = method;
    }

    int getOrderId() const { return orderId; }
    string getPaymentMethod() const { return paymentMethod; }

    void display() const
    {
        double total = 0;
        for (int i = 0; i < itemCount; i++)
        {
            total += items[i].getProduct().getPrice() * items[i].getQuantity();
        }
        cout << "\nOrder ID: " << orderId << "\n";
        cout << "Total Amount: " << total << "\n";
        cout << "Payment Method: " << paymentMethod << "\n";
        cout << "Order Details: \n";
        cout << left << setw(20) << "Product ID"
             << setw(20) << "Name"
             << setw(20) << "Price"
             << setw(20) << "Quantity" << endl;
        for (int i = 0; i < itemCount; i++)
        {
            cout << left << setw(20) << items[i].getProduct().getId()
                 << setw(20) << items[i].getProduct().getName()
                 << setw(20) << fixed << setprecision(2) << items[i].getProduct().getPrice()
                 << setw(20) << items[i].getQuantity() << endl;
        }
    }
};

// --- OrderManager Singleton ---
class OrderManager
{
private:
    static OrderManager *instance;
    Order orders[20];
    int orderCount;
    int lastOrderId;

    OrderManager() : orderCount(0) {}

public:
    static OrderManager *getInstance()
    {
        if (!instance)
            instance = new OrderManager();
        return instance;
    }

    void loadLastOrderId()
    {
        fstream infile;
        infile.open("order_id.txt", ios::in);
        if (infile.is_open())
        {
            infile >> lastOrderId;
            infile.close();
            return;
        }

        lastOrderId = 0; // if file doesn't exist
    }

    void saveLastOrderId()
    {
        fstream outfile;
        outfile.open("order_id.txt", ios::out);
        if (outfile.is_open())
        {
            outfile << lastOrderId;
            outfile.close();
        }
    }

    int getNextOrderId()
    {
        loadLastOrderId();
        lastOrderId++;
        saveLastOrderId();
        return lastOrderId;
    }

    void addOrder(Order order)
    {
        if (orderCount < 20)
        {
            orders[orderCount] = order;
            orderCount++;
            // Log to file
            fstream orderLogs;
            orderLogs.open("order_logs.txt", ios::app);
            if (orderLogs.is_open())
            {
                orderLogs << "Order ID: " << order.getOrderId()
                          << " has been successfully checked out and paid using "
                          << order.getPaymentMethod() << "." << endl;
                orderLogs.close();
            }
        }
    }

    void viewOrders()
    {
        if (orderCount == 0)
        {
            cout << "\nNo orders yet." << endl;
            return;
        }
        for (int i = 0; i < orderCount; i++)
        {
            orders[i].display();
        }
    }
};

// Initialize static member
OrderManager *OrderManager::instance = 0;

// --- ShoppingCart Class ---
class ShoppingCart
{
private:
    CartItem items[20];
    int itemCount;

public:
    ShoppingCart() : itemCount(0) {}

    void addProduct(Product product)
    {
        for (int i = 0; i < itemCount; i++)
        {
            if (items[i].getProduct().getId() == product.getId())
            {
                items[i].incrementQuantity();
                return;
            }
        }
        if (itemCount < 20)
        {
            items[itemCount] = CartItem(product, 1);
            itemCount++;
        }
    }

    void viewCart()
    {
        if (itemCount == 0)
        {
            cout << "\nShopping cart is empty.\n";
            return;
        }
        cout << "\n"
             << left << setw(20) << "Product ID"
             << setw(20) << "Name"
             << setw(20) << "Price"
             << setw(20) << "Quantity" << "\n"
             << "===================================================================\n";
        for (int i = 0; i < itemCount; i++)
        {
            cout << left << setw(20) << items[i].getProduct().getId()
                 << setw(20) << items[i].getProduct().getName()
                 << setw(20) << fixed << setprecision(2) << items[i].getProduct().getPrice()
                 << setw(20) << items[i].getQuantity() << endl;
        }
    }

    int getPaymentMethodChoice()
    {
        string input;
        while (true)
        {
            try
            {
                cout << "\nEnter your choice (1|2|3): ";
                getline(cin, input);

                if (input.empty() || input.length() != 1 || !isdigit(input[0]))
                {
                    throw invalid_argument("Invalid input. Please enter a single digit between 1 and 3.");
                }

                int choice = stoi(input);
                if (choice < 1 || choice > 3)
                {
                    throw out_of_range("Choice out of range. Please enter a number between 1 and 3.");
                }

                return choice;
            }
            catch (const invalid_argument &e)
            {
                cout << "\nError: " << e.what() << endl;
            }
            catch (const out_of_range &e)
            {
                cout << "\nError: " << e.what() << endl;
            }
        }

        return 0;
    }

    void checkout(PaymentContext &paymentContext)
    {
        double totalAmount = 0;
        for (int i = 0; i < itemCount; i++)
        {
            totalAmount += items[i].getProduct().getPrice() * items[i].getQuantity();
        }

        cout << "\nTotal Amount: " << totalAmount;
        cout << "\n\nSelect Payment Method (1: Cash, 2: Card, 3: GCash)";
        int paymentMethodChoice = getPaymentMethodChoice();

        PaymentStrategy *strategy = nullptr;
        if (paymentMethodChoice == 1)
            strategy = new CashPayment();
        else if (paymentMethodChoice == 2)
            strategy = new CardPayment();
        else if (paymentMethodChoice == 3)
            strategy = new GCashPayment();
        else
        {
            cout << "Invalid choice." << endl;
            return;
        }

        paymentContext.setStrategy(strategy);
        string paymentMethod = paymentContext.pay(totalAmount);

        Order newOrder(OrderManager::getInstance()->getNextOrderId());
        for (int i = 0; i < itemCount; i++)
        {
            newOrder.addItem(items[i]);
        }
        newOrder.setPaymentMethod(paymentMethod);

        OrderManager::getInstance()->addOrder(newOrder);
        cout << "\nYou have successfully checked out the products!" << endl;

        itemCount = 0; // Clear cart
        delete strategy;
    }

    bool isEmpty() const
    {
        return itemCount == 0;
    }
};

// --- Store Class (Main Menu) ---
class Store
{
private:
    Product products[5];
    int productCount;
    ShoppingCart cart;
    PaymentContext paymentContext;

public:
    Store() : productCount(5)
    {
        products[0] = Product("QWE", "Paper", 20.50);
        products[1] = Product("ASD", "Pencil", 10.45);
        products[2] = Product("ZXC", "Sharpener", 10.99);
        products[3] = Product("RTY", "Ballpen", 30.25);
        products[4] = Product("FGH", "Ruler", 99.99);
    }

    void viewProducts()
    {
        cout << "\n"
             << left << setw(20) << "Product ID"
             << setw(20) << "Name"
             << setw(20) << "Price" << "\n"
             << "=============================================\n";
        for (int i = 0; i < productCount; i++)
        {
            cout << left << setw(20) << products[i].getId()
                 << setw(20) << products[i].getName()
                 << setw(20) << fixed << setprecision(2) << products[i].getPrice() << endl;
        }
    }

    Product *findProductById(string id)
    {
        for (int i = 0; i < productCount; i++)
        {
            if (products[i].getId() == id)
                return &products[i];
        }
        return nullptr;
    }

    int getMenuChoice()
    {
        string input;
        bool invalidChoice = true;

        while (invalidChoice)
        {
            try
            {
                cout << "\nEnter your choice (1|2|3|4): ";
                getline(cin, input);

                if (input.empty() || input.length() != 1 || !isdigit(input[0]))
                {
                    throw invalid_argument("Invalid input. Please enter a single digit between 1 and 4.");
                }

                int choice = stoi(input);
                if (choice < 1 || choice > 4)
                {
                    throw out_of_range("Choice out of range. Please enter a number between 1 and 4.");
                }

                return choice;
            }
            catch (const invalid_argument &e)
            {
                cout << "\nError: " << e.what() << endl;
            }
            catch (const out_of_range &e)
            {
                cout << "\nError: " << e.what() << endl;
            }
        }

        return 0;
    }

    char getAnotherChoice()
    {
        string choice;
        try
        {
            cout << "\nAdd another product? (Y/N): ";
            getline(cin, choice);
            for (char &c : choice)
            {
                c = toupper(c);
            }

            if (choice != "Y" && choice != "N")
            {
                throw invalid_argument("\nInvalid input. Please enter 'Y' or 'N'.");
            }
        }
        catch (const invalid_argument &e)
        {
            cout << e.what() << "\n";
            return getAnotherChoice();
        }
        return choice[0];
    }

    char getCheckoutChoice()
    {
        string choice;
        try
        {
            cout << "\nDo you want to check out all products? (Y/N): ";
            getline(cin, choice);
            for (char &c : choice)
            {
                c = toupper(c);
            }

            if (choice != "Y" && choice != "N")
            {
                throw invalid_argument("\nInvalid input. Please enter 'Y' or 'N'.");
            }
        }
        catch (const invalid_argument &e)
        {
            cout << e.what() << "\n";
            return getCheckoutChoice();
        }
        return choice[0];
    }

    void run()
    {
        int menuChoice;
        bool menuRunning = true;
        while (menuRunning)
        {
            cout << "\n1. View Products\n2. View Shopping Cart\n3. View Orders\n4. Exit";
            menuChoice = getMenuChoice();

            if (menuChoice == 1)
            {
                viewProducts();
                string id;
                char another;
                do
                {
                    try
                    {
                        cout << "\nEnter the ID of the product to add to cart: ";
                        getline(cin, id);

                        for (char &c : id)
                        {
                            c = toupper(c);
                        }

                        Product *p = findProductById(id);
                        if (p)
                        {
                            cart.addProduct(*p);
                            cout << "\nProduct added successfully!" << endl;
                        }
                        else
                        {
                            throw invalid_argument("Product not found!");
                        }
                    }
                    catch (const invalid_argument &e)
                    {
                        cout << "\nError: " << e.what() << endl;
                    }

                    another = getAnotherChoice();
                } while (another == 'Y' || another == 'y');
            }
            else if (menuChoice == 2)
            {
                cart.viewCart();
                if (!cart.isEmpty())
                {
                    char checkoutChoice = getCheckoutChoice();
                    if (checkoutChoice == 'Y' || checkoutChoice == 'y')
                    {
                        cart.checkout(paymentContext);
                    }
                }
            }
            else if (menuChoice == 3)
            {
                OrderManager::getInstance()->viewOrders();
            }
            else if (menuChoice == 4)
            {
                cout << "Exiting..." << endl;
                break;
            }
            else
            {
                cout << "Invalid choice!" << endl;
            }
        }
    }
};

// --- Main Function ---
int main()
{
    Store store;
    store.run();
    return 0;
}