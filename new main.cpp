#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <mysql.h> // libmysql
#include <sstream> // For stringstream
#include <limits>
#include <algorithm> // For transform
#include <fstream> // Include for file operations
#include <cstdlib> // For system()
#include <set> // Include set for displayedSellers

using namespace std;

// Structs
struct User {
    int id;
    string username;
    string email;
    string password;
    string fullName;
    string address;
    string phoneNumber;
    int UserRole; // 0 = Customer, 1 = Seller, 2 = Admin
    bool isApproved; // New member to track approval status for sellers
    string storeName; // Store name for sellers
};

struct CartItem {
    int productId;      // Product ID
    int quantity;       // Quantity of the product
    double unitPrice;   // Price per unit
    string productName; // Name of the product
    string storeName;   // Store name
};

struct Product {
    string userID;
    string productID;
    string name;
    string description;
    double price;
    int stockQuantity;
    string categoryID;
    string storeName; // Store name for the product
};

struct Order {
    string orderID;
    string userID;
    double totalAmount;
    string status;
    string date; // Date of the order
    vector<CartItem> items; // List of items in the order
};

struct Transaction {
    int transactionID;
    string userID;
    double amount;
    string type; // Transaction type (e.g., "top-up", "purchase", etc.)
    string date;
};

// Minimal bcrypt functions implementation
namespace bcrypt {
    const int BCRYPT_HASHSIZE = 61;

    string generateHash(const string& password) {
        // This is a simplified example. Use an actual bcrypt library for production.
        // Placeholder for hashing function
        // In a real scenario, use a proper bcrypt library like bcrypt-cpp
        return "hashed_" + password;
    }

    bool validatePassword(const string& password, const string& hash) {
        // This is a simplified example. Use an actual bcrypt library for production.
        // Placeholder for validation function
        // In a real scenario, use a proper bcrypt library like bcrypt-cpp
        return hash == "hashed_" + password;
    }
}

// Global Variables
MYSQL* conn; // Global connection object
string glbStr; // Global string for user identification
vector<CartItem> cart; // Global variable to hold cart items
double totalAmount = 0; // To track the total amount during checkout
string currentUserID; // To store the ID of the currently logged-in user
int currentUserRole;  // To store the role of the currently logged-in user (0 = Customer, 1 = Seller, 2 = Admin)

// Function Declarations
void connectToDatabase();
bool executeUpdate(const string& query);
MYSQL_RES* executeSelectQuery(const string& query);
void clearScreen();
void displayBanner();
void mainMenu();
void registerUser();
void signInAccount();
void adminMenu();
void sellerMenu();
void customerMenu();
void manageProducts();
void approveSellers();
void manageCoupons();
void monitorWalletTransactions();
void updateCommissionRate();
void updateCommissionRateInterface();
void viewPlatformSalesReports();
void viewPlatformSalesReportsInterface();
void manageMyProducts();
void browseProducts();
void approveCustomerOrders();
void viewWalletBalance();
void viewSalesReports();
void viewTransactionHistory();
void viewCart();
void applyCoupon();
void checkout();
void viewReceipt();
void exportSalesDataToCSV();
void generateHTMLReport();
void generateHTMLReportInterface();
void eWalletManagement();
void createEWalletAccount();
void topUpWallet();
void updateOrderStatus(const string& orderID, const string& newStatusID);
void viewOrderStatus(const string& userID);
void generateSalesReport(const string& sellerID);
void viewCustomerDemographics(const string& sellerID);
void viewProductPopularity(const string& sellerID);     
void viewCartWithChatOption();
void listSellersForChat(int customerID);
void manageChatsWithSellers();
void listCustomersForChat(int sellerID);
void manageChatsWithCustomers();
void cancelOrder(const string& orderID);
void viewSalesTrends(const string& sellerID);
void viewAndUpdateProfile(const string& userID);

void manageProfileInterface();
void manageChatsWithCustomersInterface();
void approveCustomerOrdersInterface();
void viewWalletBalanceInterface();
void viewSalesReportsInterface();
void viewTransactionHistoryInterface();
void sellerSalesReportInterface();
void browseProductsInterface();
void generateSalesReportInterface();
void viewCustomerDemographicsInterface();
void viewProductPopularityInterface();
void Logout();

void myWalletInterface(); 
void messagePageInterface();
void viewProductDetails(int productId);
void myCartInterface();
void checkoutInterface();
void paymentTransaction(int orderId);
void generateReceipt(int orderId);
void viewProfile();
void editProfile();
void listSellers();
void myOrderInterface();
void viewOrderHistoryInterface();

void startNewMessage();
void selectMessage();

void viewMessages(int userID1, int userID2);
void sendMessage(int senderID, int receiverID, const std::string& messageText);
void manageProfile();
void updateProfile(const string& userID);
void viewProfile(const string& userID);

void addProduct();
void updateProduct();
void deleteProduct(); 
void searchProducts();
void sortProducts();
void viewAllProducts();

void createCoupon();
void deleteCoupon();
void updateCoupon();
void viewCoupons();

string fixedPrice(double value);


// Utility Functions
void clearScreen() {
#ifdef _WIN32
    system("cls"); // Windows
#else
    system("clear"); // Unix/Linux
#endif
}

void displayBanner() {
    cout << "=====================================================\n";
    cout << "              WELCOME TO SHOPPAY SYSTEM              \n";
    cout << "       A PLACE WITH PEACE OF MIND TO SHOPPING        \n";
    cout << "=====================================================\n\n";
}

// Function to format a double value to two decimal places
string fixedPrice(double value) {
    stringstream stream;
    stream << fixed << setprecision(2) << value;
    return stream.str();
}

// Database Functions
void connectToDatabase() {
    conn = mysql_init(nullptr);
    if (!conn) {
        cerr << "MySQL Initialization Failed!\n";
        exit(EXIT_FAILURE);
    }
    conn = mysql_real_connect(conn, "localhost", "root", "", "shoppay", 3306, nullptr, 0);
    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << "\n";
        exit(EXIT_FAILURE);
    }
    else {
        cout << "Connected to the database successfully!\n";
    }
}

bool executeUpdate(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        return false;
    }
    return true;
}

MYSQL_RES* executeSelectQuery(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        return nullptr;
    }
    return mysql_store_result(conn);
}

// Main Menu
void mainMenu() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "[1] Register/Sign Up Account\n";
        cout << "[2] Sign In Account\n";
        cout << "[3] Exit\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            clearScreen();
            registerUser();
            break;
        case 2:
            clearScreen();
            signInAccount();
            break;
        case 3:
            cout << "Exiting the system. Goodbye!\n";
            return;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

// Register/Sign Up Account
void registerUser() {
    User newUser;
    string passKey;
    int roleChoice;
    bool proceedWithRegistration = true; // Flag to check if registration should proceed

    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== REGISTER ====================\n";
        cout << "[1] Continue Registration\n";
        cout << "[2] Back to Main Menu\n";
        cout << "==================================================\n";
        cout << "Select an option: ";
        cin >> roleChoice;

        if (roleChoice == 2) {
            return; // Go back to the main menu
        }
        else if (roleChoice != 1) {
            cout << "Invalid option. Please try again.\n";
            continue;
        }

        clearScreen();
        cout << "=================== REGISTRATION ==================\n";
        cout << left << setw(20) << "Field" << setw(2) << ": " << "Input" << endl;
        cout << "---------------------------------------------------\n";
        cout << left << setw(20) << "Enter Username" << setw(2) << ": ";
        cin.ignore(); // To ignore any leftover newline character from previous input
        getline(cin, newUser.username);

        cout << left << setw(20) << "Enter Email" << setw(2) << ": ";
        getline(cin, newUser.email);

        cout << left << setw(20) << "Enter Password" << setw(2) << ": ";
        getline(cin, newUser.password);
        newUser.password = bcrypt::generateHash(newUser.password); // Hash the password

        cout << left << setw(20) << "Enter Full Name" << setw(2) << ": ";
        getline(cin, newUser.fullName);

        cout << left << setw(20) << "Enter Address" << setw(2) << ": ";
        getline(cin, newUser.address);

        cout << left << setw(20) << "Enter Phone Number" << setw(2) << ": ";
        getline(cin, newUser.phoneNumber);
        newUser.isApproved = false; // Default to not approved

        cout << left << setw(20) << "Select User Role" << setw(2) << ": \n[1] Admin\n[2] Seller\n[3] Customer\n";
        cin >> roleChoice;

        if (roleChoice == 1) {
            bool correctPassKey = false;
            while (!correctPassKey) {
                cin.ignore(); // To ignore any leftover newline character from previous input
                cout << "Enter the Admin Pass Key: ";
                getline(cin, passKey);
                if (passKey == "Shoppay System") {
                    newUser.UserRole = 2; // Set as admin
                    correctPassKey = true;
                }
                else {
                    cout << "Incorrect pass key.\n";
                    int retryChoice;
                    cout << "[1] Try Again\n[2] Cancel/Back\n";
                    cin >> retryChoice;
                    if (retryChoice == 2) {
                        cout << "Your Registration as Admin Failed. Please Try Again later.\n";
                        proceedWithRegistration = false; // Set flag to false to prevent saving data
                        break;
                    }
                }
            }
        }
        else if (roleChoice == 2) {
            newUser.UserRole = 1; // Set as seller

            cout << "Enter Store Name: ";
            cin.ignore();
            getline(cin, newUser.storeName);
        }
        else {
            newUser.UserRole = 0; // Default to customer
        }

        if (proceedWithRegistration) {
            // Insert user into the database
            string query = "INSERT INTO user (Username, Email, Password, FullName, Address, PhoneNumber, UserRole, isApproved, StoreName) VALUES ('" +
                newUser.username + "', '" + newUser.email + "', '" + newUser.password + "', '" +
                newUser.fullName + "', '" + newUser.address + "', '" + newUser.phoneNumber + "', " +
                to_string(newUser.UserRole) + ", " + to_string(newUser.isApproved) + ", '" + newUser.storeName + "')";
            if (!executeUpdate(query)) {
                cout << "Error: " << mysql_error(conn) << endl;
            }
            else {
                clearScreen();
                displayBanner();
                cout << "================ REGISTRATION SUCCESS ================\n";
                cout << "Registration successful!\n";
                cout << "=====================================================\n";
            }
        }
        else {
            cout << "Registration was not completed.\n";
        }

        break;
    }
}

// Sign In Account
// Sign In Account
void signInAccount() {
    string username, password;

    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== SIGN IN ====================\n";
        cout << left << setw(20) << "Field" << setw(2) << ": " << "Input" << endl;
        cout << "-------------------------------------------------\n";

        cout << left << setw(20) << "Enter Username" << setw(2) << ": ";
        cin.ignore(); // To ignore any leftover newline character from previous input
        getline(cin, username);

        cout << left << setw(20) << "Enter Password" << setw(2) << ": ";
        getline(cin, password);

        // Query to get user details
        string query = "SELECT UserID, UserRole, isApproved, Password FROM user WHERE Username = '" + username + "'";
        MYSQL_RES* res = executeSelectQuery(query);

        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {
                int userID = atoi(row[0]);
                int userRole = atoi(row[1]);
                int isApproved = atoi(row[2]);
                string storedPassword = row[3];

                if (bcrypt::validatePassword(password, storedPassword)) {
                    if (userRole == 1 && !isApproved) {
                        clearScreen();
                        cout << "================ SIGN IN FAILED ================\n";
                        cout << "Login failed: Your account is not approved yet.\n";
                        cout << "=================================================\n";
                    }
                    else {
                        glbStr = to_string(userID); // Store UserID in global variable
                        currentUserID = to_string(userID); // Ensure currentUserID is set
                        clearScreen();
                        displayBanner();
                        cout << "================ SIGN IN SUCCESS ================\n";
                        cout << "Welcome back, " << username << "!\n";
                        cout << "=================================================\n";
                        cin.ignore();
                        cin.get();

                        if (userRole == 2) {
                            adminMenu();
                        }
                        else if (userRole == 1) {
                            sellerMenu();
                        }
                        else if (userRole == 0) {
                            customerMenu();
                        }
                    }
                }
                else {
                    cout << "Invalid username or password.\n";
                }
            }
            else {
                cout << "Invalid username or password.\n";
            }
            mysql_free_result(res);
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        // Option to retry or go back to the main menu
        int retryChoice;
        cout << "==================== OPTIONS ====================\n";
        cout << "[1] Try Again\n";
        cout << "[2] Back to Main Menu\n";
        cout << "=================================================\n";
        cout << "Select an option: ";
        cin >> retryChoice;

        if (retryChoice == 2) {
            mainMenu(); // Go back to the main menu
        }
    }
}

// Admin Menu
void adminMenu() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "[1] Products Details\n";
        cout << "[2] Approve Registered Sellers\n";
        cout << "[3] Coupon Management\n";
        cout << "[4] Wallet Transactions Monitor\n";
        cout << "[5] Commission Rate\n";
        cout << "[6] Platform Sales Reports\n";
        cout << "[7] Graph Reports\n";
        cout << "[8] Logout\n";
        cout << "\nSelect an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            manageProducts();
            break;
        case 2:
            approveSellers();
            break;
        case 3:
            manageCoupons();
            break;
        case 4:
            monitorWalletTransactions();
            break;
        case 5:
            updateCommissionRateInterface();
            break;
        case 6:
            viewPlatformSalesReportsInterface();
            break;
        case 7:
            generateHTMLReportInterface();
            break;
        case 8:
            Logout();
            break;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

// Seller Menu
void sellerMenu() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== SELLER MENU ====================\n";
        cout << "| [1] My Profile                                    |\n";
        cout << "| [2] View Messages                                 |\n";
        cout << "| [3] Manage Products                               |\n";
        cout << "| [4] Approve Customer Orders                       |\n";
        cout << "| [5] Wallet Balance                                |\n";
        cout << "| [6] Sales Reports                                 |\n";
        cout << "| [7] Transaction History                           |\n";
        cout << "| [8] Seller Sales Report                           |\n";
        cout << "| [9] Logout                                        |\n";
        cout << "=====================================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            viewAndUpdateProfile(currentUserID);
            break;
        case 2:
            manageChatsWithCustomersInterface();
            break;
        case 3:
            manageProducts();
            break;
        case 4:
            approveCustomerOrdersInterface();
            break;
        case 5:
            viewWalletBalanceInterface();
            break;
        case 6:
            viewSalesReportsInterface();
            break;
        case 7:
            viewTransactionHistoryInterface();
            break;
        case 8:
            sellerSalesReportInterface();
            break;
        case 9:
            Logout(); // Return to main menu
            break;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

// Customer Menu
void customerMenu() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== CUSTOMER MENU ====================\n";
        cout << "| [1] My Profile (View/Edit)                           |\n";
        cout << "| [2] My Wallet (Balance/Top-Up)                       |\n";
        cout << "| [3] Message Page (Message Sellers)                   |\n";
        cout << "| [4] Browse Products                                  |\n";
        cout << "| [5] My Cart                                          |\n";
        cout << "| [6] Checkout                                         |\n";
        cout << "| [7] My Order                                         |\n";
        cout << "| [8] View Order History                               |\n";
        cout << "| [9] Logout                                           |\n";
        cout << "=======================================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            viewAndUpdateProfile(currentUserID);
            break;
        case 2:
            myWalletInterface();
            break;
        case 3:
            messagePageInterface();
            break;
        case 4:
            browseProductsInterface();
            break;
        case 5:
            myCartInterface();
            break;
        case 6:
            checkoutInterface();
            break;
        case 7:
            myOrderInterface();
            break;
        case 8:
            viewOrderHistoryInterface();
            break;
        case 9:
            Logout(); // Return to main menu
            break;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

// Implement remaining functions for managing products, approving sellers, managing coupons, etc.
void manageProducts() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "================= MANAGE PRODUCTS =================\n";
        cout << "[1] Add Product\n";
        cout << "[2] Update Product\n";
        cout << "[3] Delete Product\n";
        cout << "[4] View All Products\n";
        cout << "[5] Search Products\n";
        cout << "[6] Sort Products\n";
        cout << "[7] Browse Products (Monitor Other Sellers)\n";
        cout << "[8] Back to Admin Menu\n";
        cout << "===================================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            addProduct();
            break;
        case 2:
            updateProduct();
            break;
        case 3:
            deleteProduct();
            break;
        case 4:
            viewAllProducts();
            break;
        case 5:
            searchProducts();
            break;
        case 6:
            sortProducts();
            break;
        case 7:
            browseProductsInterface(); 
        case 8:
            adminMenu(); // Return to main menu;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

void addProduct() {
    Product newProduct;
    string name, description, storeName;
    double price;
    int stock;

    cout << "========== ADD PRODUCT ==========\n";
    cout << "Enter Product Name: ";
    cin.ignore();
    getline(cin, name);
    newProduct.name = name;

    cout << "Enter Product Description: ";
    getline(cin, description);
    newProduct.description = description;

    cout << "Enter Product Price: $";
    cin >> price;
    newProduct.price = price;

    cout << "Enter Stock Quantity: ";
    cin >> stock;
    newProduct.stockQuantity = stock;

    cout << "Enter Store Name: ";
    cin.ignore();
    getline(cin, storeName);
    newProduct.storeName = storeName;

    string query = "INSERT INTO product (ProductName, Description, Price, StockQuantity, SellerID, StoreName) VALUES ('" +
        newProduct.name + "', '" + newProduct.description + "', " + to_string(newProduct.price) + ", " +
        to_string(newProduct.stockQuantity) + ", " + glbStr + ", '" + newProduct.storeName + "')";
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product added successfully!\n";
    }
}

void updateProduct() {
    int productId;
    string name, description, storeName;
    double price;
    int stock;

    cout << "========== UPDATE PRODUCT ==========\n";
    cout << "Enter Product ID to update: ";
    cin >> productId;
    cout << "Enter New Product Name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter New Product Description: ";
    getline(cin, description);
    cout << "Enter New Product Price: $";
    cin >> price;
    cout << "Enter New Stock Quantity: ";
    cin >> stock;
    cout << "Enter Store Name: ";
    cin.ignore();
    getline(cin, storeName);

    string query = "UPDATE product SET ProductName = '" + name +
        "', Description = '" + description +
        "', Price = " + to_string(price) +
        ", StockQuantity = " + to_string(stock) +
        ", StoreName = '" + storeName +
        "' WHERE ProductID = " + to_string(productId);
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product updated successfully!\n";
    }
}

void deleteProduct() {
    int productId;

    cout << "========== DELETE PRODUCT ==========\n";
    cout << "Enter Product ID to delete: ";
    cin >> productId;

    string query = "DELETE FROM product WHERE ProductID = " + to_string(productId);
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product deleted successfully!\n";
    }
}

void viewAllProducts() {
    string query = "SELECT * FROM product";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
            << setw(10) << "Price" << setw(10) << "Stock" << endl;
        cout << string(100, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
                << setw(10) << row[3] << setw(10) << row[4] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void searchProducts() {
    string keyword, category, minPrice, maxPrice, minRating;

    cout << "Enter keyword: ";
    cin.ignore();
    getline(cin, keyword);

    cout << "Enter category (or leave blank): ";
    getline(cin, category);

    cout << "Enter minimum price (or leave blank): ";
    getline(cin, minPrice);

    cout << "Enter maximum price (or leave blank): ";
    getline(cin, maxPrice);

    cout << "Enter minimum rating (or leave blank): ";
    getline(cin, minRating);

    string query = "SELECT p.ProductID, p.ProductName, p.Description, p.Price, p.StockQuantity, AVG(r.Rating) as AvgRating "
        "FROM product p LEFT JOIN reviews r ON p.ProductID = r.ProductID "
        "WHERE p.ProductName LIKE '%" + keyword + "%' ";

    if (!category.empty()) {
        query += "AND p.CategoryID = (SELECT CategoryID FROM category WHERE CategoryName = '" + category + "') ";
    }
    if (!minPrice.empty()) {
        query += "AND p.Price >= " + minPrice + " ";
    }
    if (!maxPrice.empty()) {
        query += "AND p.Price <= " + maxPrice + " ";
    }
    if (!minRating.empty()) {
        query += "GROUP BY p.ProductID HAVING AvgRating >= " + minRating;
    }
    else {
        query += "GROUP BY p.ProductID";
    }

    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
            << setw(10) << "Price" << setw(10) << "Stock" << setw(10) << "AvgRating" << endl;
        cout << string(110, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
                << setw(10) << row[3] << setw(10) << row[4] << setw(10) << row[5] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void sortProducts() {
    int choice;
    string query;

    cout << "========== SORT PRODUCTS ==========\n";
    cout << "[1] Sort by Price\n";
    cout << "[2] Sort by Name\n";
    cout << "Select an option: ";
    cin >> choice;

    if (choice == 1) {
        query = "SELECT * FROM product ORDER BY Price";
    }
    else if (choice == 2) {
        query = "SELECT * FROM product ORDER BY ProductName";
    }
    else {
        cout << "Invalid option.\n";
        return;
    }

    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
            << setw(10) << "Price" << setw(10) << "Stock" << endl;
        cout << string(100, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
                << setw(10) << row[3] << setw(10) << row[4] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void approveSellers() {
    clearScreen();
    cout << "========== APPROVE REGISTERED SELLERS ==========\n";

    string query = "SELECT UserID, Username, Email, FullName, PhoneNumber, RegistrationDate FROM user WHERE UserRole = 1 AND isApproved = 0";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(8) << "UserID" << setw(18) << "Username" << setw(32) << "Email"
            << setw(25) << "Full Name" << setw(15) << "Phone Number" << setw(20) << "Registration Date" << endl;
        cout << string(118, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(8) << row[0] << setw(18) << row[1] << setw(32) << row[2]
                << setw(25) << row[3] << setw(15) << row[4] << setw(20) << row[5] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    int userID;
    char decision;
    cout << "\nEnter UserID to approve or decline (or '0' to go back): ";
    cin >> userID;
    if (userID == 0) return;

    cout << "Approve (y/n)? ";
    cin >> decision;

    string update_query;
    if (tolower(decision) == 'y') {
        update_query = "UPDATE user SET isApproved = 1 WHERE UserID = " + to_string(userID);
    }
    else {
        update_query = "DELETE FROM user WHERE UserID = " + to_string(userID);
    }

    if (executeUpdate(update_query)) {
        cout << "\n====================\n";
        cout << (tolower(decision) == 'y' ? "Seller approved!" : "Seller declined and deleted!") << endl;
        cout << "====================\n";
    }
    else {
        cerr << "Failed to update seller status." << endl;
    }

    cout << "Press Enter to return...";
    cin.ignore();
    cin.get();
}
void manageCoupons() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "========== COUPON MANAGEMENT ==========\n";
        cout << "[1] Create Coupon\n";
        cout << "[2] View All Coupons\n";
        cout << "[3] Update Coupon\n";
        cout << "[4] Delete Coupon\n";
        cout << "[5] Back to Admin Menu\n";
        cout << "=======================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            createCoupon();
            break;
        case 2:
            viewCoupons();
            break;
        case 3:
            updateCoupon();
            break;
        case 4:
            deleteCoupon();
            break;
        case 5:
            return;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

void createCoupon() {
    string code, validFrom, validUntil;
    double discountPercentage, minPurchase;
    int usageLimit;

    cout << "Enter Coupon Code: ";
    cin >> code;
    cout << "Enter Discount Percentage (0-100): ";
    cin >> discountPercentage;
    cout << "Enter Valid From (YYYY-MM-DD): ";
    cin >> validFrom;
    cout << "Enter Valid Until (YYYY-MM-DD): ";
    cin >> validUntil;
    cout << "Enter Minimum Purchase Amount: $";
    cin >> minPurchase;
    cout << "Enter Usage Limit (-1 for unlimited): ";
    cin >> usageLimit;

    string query = "INSERT INTO coupon (Code, DiscountPercentage, ValidFrom, ValidUntil, MinPurchase, UsageLimit) VALUES ('" +
        code + "', " + to_string(discountPercentage) + ", '" + validFrom + "', '" + validUntil + "', " +
        to_string(minPurchase) + ", " + (usageLimit == -1 ? "NULL" : to_string(usageLimit)) + ")";
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Coupon created successfully!\n";
    }
}

void viewCoupons() {
    string query = "SELECT * FROM coupon";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "CouponID" << setw(20) << "Code" << setw(15) << "Discount"
            << setw(20) << "Valid From" << setw(20) << "Valid Until" << setw(15) << "Min Purchase"
            << setw(15) << "Usage Limit" << endl;
        cout << string(100, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(15) << row[2]
                << setw(20) << row[3] << setw(20) << row[4] << setw(15) << row[5]
                << setw(15) << (row[6] ? row[6] : "Unlimited") << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void updateCoupon() {
    int couponID;
    string validUntil;

    cout << "Enter Coupon ID to update: ";
    cin >> couponID;
    cout << "Enter new Valid Until date (YYYY-MM-DD): ";
    cin >> validUntil;

    string query = "UPDATE coupon SET ValidUntil = '" + validUntil + "' WHERE CouponID = " + to_string(couponID);
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Coupon updated successfully!\n";
    }
}

void deleteCoupon() {
    int couponID;
    cout << "Enter Coupon ID to delete: ";
    cin >> couponID;

    string query = "DELETE FROM coupon WHERE CouponID = " + to_string(couponID);
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Coupon deleted successfully!\n";
    }
}

void monitorWalletTransactions() {
    while (true) {
        clearScreen();
        cout << "========== MONITOR ALL WALLET TRANSACTIONS ==========\n";
        cout << "[1] View Transactions\n";
        cout << "[2] Back to Admin Menu\n";
        cout << "=====================================================\n";
        cout << "Select an option: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            string query = "SELECT TransactionID, WalletID, Amount, TransactionType, TransactionDate FROM transaction ORDER BY TransactionDate DESC";
            MYSQL_RES* res = executeSelectQuery(query);

            if (res) {
                MYSQL_ROW row;
                cout << left << setw(15) << "TransactionID" << setw(15) << "WalletID"
                    << setw(15) << "Amount" << setw(15) << "Type" << setw(30) << "Date" << endl;
                cout << string(90, '-') << endl;

                while ((row = mysql_fetch_row(res))) {
                    cout << left << setw(15) << row[0] << setw(15) << row[1]
                        << setw(15) << "$" + fixedPrice(stod(row[2])) << setw(15) << row[3]
                        << setw(30) << row[4] << endl;
                }
                mysql_free_result(res);
            }
            else {
                cout << "Error: " << mysql_error(conn) << endl;
            }
        }
        else if (choice == 2) {
            return;  // Back to Admin Menu
        }
        else {
            cout << "Invalid option. Please try again." << endl;
        }

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

void updateCommissionRate() {
    clearScreen();
    double newRate;
    cout << "Enter new commission rate (in %): ";
    cin >> newRate;

    if (newRate < 0 || newRate > 100) {
        cout << "Invalid rate. Please enter a value between 0 and 100." << endl;
        return;
    }

    string query = "UPDATE system_settings SET Value = " + to_string(newRate / 100) + " WHERE SettingName = 'CommissionRate'";
    if (!executeUpdate(query)) {
        cerr << "Error updating commission rate: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Commission rate updated successfully!" << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void approveCustomerOrders() {
    clearScreen();
    cout << "========== APPROVE CUSTOMER ORDERS ==========\n";

    string query = "SELECT OrderID, UserID, TotalAmount, OrderStatus FROM orders WHERE OrderStatus = 'Pending Seller Approval' AND UserID = " + glbStr;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "OrderID" << setw(10) << "UserID" << setw(15) << "Total Amount" << setw(20) << "Order Status" << endl;
        cout << string(60, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(10) << row[1] << setw(15) << row[2] << setw(20) << row[3] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    int orderID;
    char decision;
    cout << "Enter OrderID to approve or reject (or '0' to go back): ";
    cin >> orderID;
    if (orderID == 0) return;

    cout << "Approve (y/n)? ";
    cin >> decision;

    string update_query;
    if (tolower(decision) == 'y') {
        update_query = "UPDATE orders SET OrderStatus = 'Approved' WHERE OrderID = " + to_string(orderID);
    }
    else {
        update_query = "UPDATE orders SET OrderStatus = 'Rejected' WHERE OrderID = " + to_string(orderID);
    }

    if (executeUpdate(update_query)) {
        cout << (tolower(decision) == 'y' ? "Order approved!" : "Order rejected!") << endl;
    }
    else {
        cerr << "Failed to update order status." << endl;
    }

    cout << "Press Enter to return...";
    cin.ignore();
    cin.get();
}

void viewSalesReports() {
    clearScreen();
    string query = "SELECT SUM(TotalAmount) AS TotalSales, COUNT(OrderID) AS OrderCount FROM orders WHERE UserID = " + glbStr + " AND OrderStatus = 'Approved'";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "========== SALES REPORT ==========\n";
            cout << "Total Sales: $" << fixedPrice(stod(row[0])) << endl;
            cout << "Total Orders: " << row[1] << endl;
        }
        else {
            cout << "No sales data found for your account." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void browseProducts() {
    string query = "SELECT * FROM product";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
            << setw(10) << "Price" << setw(10) << "Stock" << endl;
        cout << string(100, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
                << setw(10) << row[3] << setw(10) << row[4] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewCartWithChatOption() {
    string query = "SELECT c.ProductID, p.ProductName, p.SellerID, u.StoreName FROM cart c INNER JOIN product p ON c.ProductID = p.ProductID INNER JOIN user u ON p.SellerID = u.UserID WHERE c.UserID = " + glbStr;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << "========== YOUR CART ==========\n";
        cout << left << setw(10) << "ProductID" << setw(30) << "Product Name" << setw(10) << "SellerID" << setw(30) << "Store Name" << endl;
        cout << string(80, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(30) << row[1] << setw(10) << row[2] << setw(30) << row[3] << endl;
        }
        mysql_free_result(res);

        int productID, sellerID;
        string message;

        cout << "Enter Product ID to chat about: ";
        cin >> productID;

        // Get the SellerID for the selected product
        query = "SELECT p.SellerID FROM product p WHERE p.ProductID = " + to_string(productID);
        res = executeSelectQuery(query);
        if (res && (row = mysql_fetch_row(res))) {
            sellerID = atoi(row[0]);
            mysql_free_result(res);
        }
        else {
            cout << "Invalid Product ID. Please try again.\n";
            return;
        }

        cin.ignore(); // Clear the newline character from the input buffer
        cout << "Enter your message: ";
        getline(cin, message);

        sendMessage(stoi(glbStr), sellerID, message); // Send message from current user to seller
        viewMessages(stoi(glbStr), sellerID); // View updated chat history with the seller
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void checkout() {
    if (totalAmount > 0) {
        // Assuming 1 is the StatusID for 'Pending Seller Approval'
        string query = "INSERT INTO orders (UserID, TotalAmount, StatusID) VALUES (" + glbStr + ", " + to_string(totalAmount) + ", 1)";
        if (executeUpdate(query)) {
            cout << "Checkout successful! Your order is pending seller approval.\n";
            cart.clear();
            totalAmount = 0;
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
        }
    }
    else {
        cout << "Your cart is empty!\n";
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewTransactionHistory() {
    clearScreen();
    string query = "SELECT TransactionID, Amount, TransactionType, TransactionDate FROM transaction WHERE WalletID = (SELECT WalletID FROM wallet WHERE UserID = " + glbStr + ")";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(15) << "TransactionID" << setw(15) << "Amount" << setw(15) << "Type" << setw(30) << "Date" << endl;
        cout << string(75, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(15) << row[0] << setw(15) << "$" + fixedPrice(stod(row[1])) << setw(15) << row[2] << setw(30) << row[3] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewReceipt() {
    clearScreen();
    int orderID;
    cout << "Enter Order ID to view receipt: ";
    cin >> orderID;

    string query = "SELECT OrderID, UserID, TotalAmount, StatusID, OrderDate FROM orders WHERE OrderID = " + to_string(orderID);
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "========== RECEIPT ==========\n";
            cout << "Order ID: " << row[0] << endl;
            cout << "User ID: " << row[1] << endl;
            cout << "Total Amount: $" << fixedPrice(stod(row[2])) << endl;
            cout << "Order Status: " << row[3] << endl;
            cout << "Order Date: " << row[4] << endl;

            // Retrieve and display order items
            string itemQuery = "SELECT ProductID, Quantity, UnitPrice, ProductName, StoreName FROM order_items WHERE OrderID = " + to_string(orderID);
            MYSQL_RES* itemRes = executeSelectQuery(itemQuery);
            if (itemRes) {
                MYSQL_ROW itemRow;
                cout << left << setw(15) << "ProductID" << setw(15) << "ProductName" << setw(10) << "Quantity" << setw(15) << "UnitPrice" << setw(20) << "StoreName" << endl;
                cout << string(75, '-') << endl;

                while ((itemRow = mysql_fetch_row(itemRes))) {
                    cout << left << setw(15) << itemRow[0] << setw(15) << itemRow[3] << setw(10) << itemRow[1] << setw(15) << fixedPrice(stod(itemRow[2])) << setw(20) << itemRow[4] << endl;
                }
                mysql_free_result(itemRes);
            }
        }
        else {
            cout << "No receipt found for the provided Order ID.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void exportSalesDataToCSV() {
    string query = "SELECT * FROM orders WHERE OrderStatus = 'Approved'";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        ofstream file("sales_data.csv");
        if (file.is_open()) {
            file << "OrderID,UserID,TotalAmount,OrderStatus,OrderDate\n";
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                file << row[0] << "," << row[1] << "," << fixedPrice(stod(row[2])) << "," << row[3] << "," << row[4] << "\n";
            }
            file.close();
            cout << "Sales data exported to sales_data.csv successfully.\n";
        }
        else {
            cout << "Unable to open file for writing.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void generateHTMLReport() {
    clearScreen();
    string query = "SELECT DATE(OrderDate) as OrderDate, SUM(TotalAmount) as DailySales FROM orders WHERE OrderStatus = 'Approved' GROUP BY DATE(OrderDate)";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        ofstream file("sales_report.html");
        if (file.is_open()) {
            file << R"(
<!DOCTYPE html>
<html>
<head>
    <title>Sales Report</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <h1>Sales Report</h1>
    <canvas id="salesChart" width="800" height="400"></canvas>
    <script>
        var ctx = document.getElementById('salesChart').getContext('2d');
        var salesChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [)";

            MYSQL_ROW row;
            bool first = true;
            while ((row = mysql_fetch_row(res))) {
                if (!first) file << ",";
                first = false;
                file << "'" << row[0] << "'";
            }

            file << R"(],
                datasets: [{
                    label: 'Daily Sales',
                    data: [)";

            mysql_data_seek(res, 0); // Reset result pointer
            first = true;
            while ((row = mysql_fetch_row(res))) {
                if (!first) file << ",";
                first = false;
                file << row[1];
            }

            file << R"(],
                    borderColor: 'rgba(75, 192, 192, 1)',
                    borderWidth: 1
                }]
            },
            options: {
                scales: {
                    y: {
                        beginAtZero: true
                    }
                }
            }
        });
    </script>
</body>
</html>
)";
            file.close();
            cout << "HTML report generated successfully at sales_report.html.\n";
        }
        else {
            cout << "Unable to open file for writing.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void eWalletManagement() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "========== E-WALLET MANAGEMENT ==========\n";
        cout << "[1] View Wallet Balance\n";
        cout << "[2] Top-Up Wallet\n";
        cout << "[3] Back to Customer Menu\n";
        cout << "=========================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            viewWalletBalance();
            break;
        case 2:
            topUpWallet();
            break;
        case 3:
            return;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

void manageMyProducts() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "================= MANAGE MY PRODUCTS =================\n";
        cout << "[1] Add Product\n";
        cout << "[2] Update Product\n";
        cout << "[3] Delete Product\n";
        cout << "[4] View My Products\n";
        cout << "[5] Back to Seller Menu\n";
        cout << "=====================================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            addProduct();
            break;
        case 2:
            updateProduct();
            break;
        case 3:
            deleteProduct();
            break;
        case 4:
            viewAllProducts();
            break;
        case 5:
            return; // Back to Seller Menu
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

void viewPlatformSalesReports() {
    clearScreen();
    string query = "SELECT SUM(TotalAmount) AS TotalSales, COUNT(OrderID) AS OrderCount FROM orders WHERE OrderStatus = 'Approved'";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "========== PLATFORM SALES REPORT ==========\n";
            cout << "Total Sales: $" << fixedPrice(stod(row[0])) << endl;
            cout << "Total Orders: " << row[1] << endl;
        }
        else {
            cout << "No sales data found.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void updateOrderStatus(const string& orderID, const string& newStatusID) {
    int orderID_int = stoi(orderID);
    int newStatusID_int = stoi(newStatusID);
    string query = "UPDATE orders SET StatusID = " + to_string(newStatusID_int) + " WHERE OrderID = " + to_string(orderID_int);
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Order status updated successfully!\n";
    }
}

void viewOrderStatus(const string& userID) {
    int userID_int = stoi(userID);
    string query = "SELECT o.OrderID, os.StatusName, o.OrderDate FROM orders o INNER JOIN order_status os ON o.StatusID = os.StatusID WHERE o.UserID = " + to_string(userID_int) + " ORDER BY o.OrderDate DESC";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "OrderID" << setw(20) << "Status" << setw(30) << "Order Date" << endl;
        cout << string(60, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(30) << row[2] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void generateSalesReport(const string& sellerID) {
    int sellerID_int = stoi(sellerID);
    string query = "SELECT SUM(oi.UnitPrice * oi.Quantity) AS TotalSales, COUNT(o.OrderID) AS OrderCount FROM orders o INNER JOIN order_items oi ON o.OrderID = oi.OrderID WHERE oi.SellerID = " + to_string(sellerID_int) + " AND o.StatusID = (SELECT StatusID FROM order_status WHERE StatusName = 'Approved')";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "Total Sales: $" << fixedPrice(stod(row[0])) << endl;
            cout << "Total Orders: " << row[1] << endl;
        }
        else {
            cout << "No sales data found for your account." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewCustomerDemographics(const string& sellerID) {
    int sellerID_int = stoi(sellerID);
    string query = "SELECT u.City, COUNT(o.OrderID) AS OrderCount FROM orders o INNER JOIN user u ON o.UserID = u.UserID INNER JOIN order_items oi ON o.OrderID = oi.OrderID WHERE oi.SellerID = " + to_string(sellerID_int) + " AND o.StatusID = (SELECT StatusID FROM order_status WHERE StatusName = 'Approved') GROUP BY u.City";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(20) << "City" << setw(20) << "Order Count" << endl;
        cout << string(40, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(20) << row[0] << setw(20) << row[1] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewProductPopularity(const string& sellerID) {
    int sellerID_int = stoi(sellerID);
    string query = "SELECT p.ProductName, SUM(oi.Quantity) AS TotalQuantity FROM order_items oi INNER JOIN product p ON oi.ProductID = p.ProductID WHERE oi.SellerID = " + to_string(sellerID_int) + " GROUP BY p.ProductName ORDER BY TotalQuantity DESC";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(30) << "Product Name" << setw(20) << "Total Quantity" << endl;
        cout << string(50, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(30) << row[0] << setw(20) << row[1] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void listSellersForChat(int customerID) {
    string query = "SELECT DISTINCT u.UserID, u.StoreName FROM messages m INNER JOIN user u ON m.ReceiverID = u.UserID WHERE m.SenderID = " + to_string(customerID) + " OR m.ReceiverID = " + to_string(customerID);
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << "========== CHAT WITH SELLERS ==========\n";
        cout << left << setw(10) << "SellerID" << setw(30) << "Store Name" << endl;
        cout << string(40, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(30) << row[1] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void manageChatsWithSellers() {
    listSellersForChat(stoi(glbStr)); // List sellers the customer has interacted with

    int sellerID;
    string message;

    cout << "Enter Seller ID to chat with: ";
    cin >> sellerID;
    cin.ignore(); // Clear the newline character from the input buffer

    // Display chat history
    viewMessages(stoi(glbStr), sellerID);

    cout << "Enter your message: ";
    getline(cin, message);

    sendMessage(stoi(glbStr), sellerID, message); // Send message from current user to seller
    viewMessages(stoi(glbStr), sellerID); // View updated chat history with the seller
}

void listCustomersForChat(int sellerID) {
    string query = "SELECT DISTINCT u.UserID, u.Username FROM messages m INNER JOIN user u ON m.SenderID = u.UserID WHERE m.ReceiverID = " + to_string(sellerID);
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << "========== CHAT WITH CUSTOMERS ==========\n";
        cout << left << setw(10) << "CustomerID" << setw(30) << "Username" << endl;
        cout << string(40, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(30) << row[1] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void manageChatsWithCustomers() {
    listCustomersForChat(stoi(glbStr)); // List customers the seller has interacted with

    int customerID;

    cout << "Enter Customer ID to view messages: ";
    cin >> customerID;

    // Display chat history
    viewMessages(stoi(glbStr), customerID);

    string reply;
    cout << "Enter your reply: ";
    cin.ignore(); // Clear the newline character from the input buffer
    getline(cin, reply);

    sendMessage(stoi(glbStr), customerID, reply); // Send reply from seller to customer
    viewMessages(stoi(glbStr), customerID); // View updated chat history with the customer
}

void viewMessages(int userID1, int userID2) {
    std::string query = "SELECT SenderID, MessageText, Timestamp FROM messages WHERE (SenderID = " + std::to_string(userID1) + " AND ReceiverID = " + std::to_string(userID2) + ") OR (SenderID = " + std::to_string(userID2) + " AND ReceiverID = " + std::to_string(userID1) + ") ORDER BY Timestamp";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Error: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        std::cerr << "Error: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        std::cout << (atoi(row[0]) == userID1 ? "You: " : "Seller: ") << row[1] << " [" << row[2] << "]\n";
    }

    mysql_free_result(res);
}

void sendMessage(int senderID, int receiverID, const std::string& messageText) {
    std::string query = "INSERT INTO messages (SenderID, ReceiverID, MessageText) VALUES (" +
        std::to_string(senderID) + ", " + std::to_string(receiverID) + ", '" + messageText + "')";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Error: " << mysql_error(conn) << std::endl;
    }
    else {
        std::cout << "Message sent successfully!\n";
    }
}

void manageProfile() {
    clearScreen();
    displayBanner();
    viewProfile(glbStr); // Display current profile details

    cout << "\nDo you want to update your profile? (y/n): ";
    char updateChoice;
    cin >> updateChoice;

    if (tolower(updateChoice) == 'y') {
        updateProfile(glbStr);
    }

    cout << "\nPress Enter to return to the menu...";
    cin.ignore();
    cin.get();
}

void viewProfile(const string& userID) {
    string query = "SELECT Username, Email, FullName, PhoneNumber FROM user WHERE UserID = " + userID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "========== PROFILE DETAILS ==========\n";
            cout << "Username: " << row[0] << endl;
            cout << "Email: " << row[1] << endl;
            cout << "Full Name: " << row[2] << endl;
            cout << "Phone Number: " << row[3] << endl;
        }
        else {
            cout << "No profile found for the provided UserID.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    query = "SELECT AddressLine1, AddressLine2, City, State, PostalCode, Country FROM address WHERE UserID = " + userID;
    res = executeSelectQuery(query);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "========== ADDRESS DETAILS ==========\n";
            cout << "Address Line 1: " << row[0] << endl;
            cout << "Address Line 2: " << row[1] << endl;
            cout << "City: " << row[2] << endl;
            cout << "State: " << row[3] << endl;
            cout << "Postal Code: " << row[4] << endl;
            cout << "Country: " << row[5] << endl;
        }
        else {
            cout << "No address found for the provided UserID.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void updateProfile(const string& userID) {
    clearScreen();
    displayBanner();
    cout << "========== UPDATE PROFILE ==========\n";

    string email, fullName, phoneNumber;

    cout << "Enter new Email: ";
    cin.ignore();
    getline(cin, email);

    cout << "Enter new Full Name: ";
    getline(cin, fullName);

    cout << "Enter new Phone Number: ";
    getline(cin, phoneNumber);

    string query = "UPDATE user SET Email = '" + email + "', FullName = '" + fullName + "', PhoneNumber = '" + phoneNumber + "' WHERE UserID = " + userID;
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Profile updated successfully!\n";
    }

    string addressLine1, addressLine2, city, state, postalCode, country;

    cout << "Enter new Address Line 1: ";
    getline(cin, addressLine1);

    cout << "Enter new Address Line 2 (or leave blank): ";
    getline(cin, addressLine2);

    cout << "Enter new City: ";
    getline(cin, city);

    cout << "Enter new State: ";
    getline(cin, state);

    cout << "Enter new Postal Code: ";
    getline(cin, postalCode);

    cout << "Enter new Country: ";
    getline(cin, country);

    query = "UPDATE address SET AddressLine1 = '" + addressLine1 + "', AddressLine2 = '" + addressLine2 + "', City = '" + city + "', State = '" + state + "', PostalCode = '" + postalCode + "', Country = '" + country + "' WHERE UserID = " + userID;
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Address updated successfully!\n";
    }
}

void cancelOrder(const string& orderID) {
    string query = "UPDATE orders SET OrderStatus = 'Cancelled' WHERE OrderID = " + orderID + " AND OrderStatus = 'Pending Seller Approval'";
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Order cancelled successfully!\n";
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewSalesTrends(const string& sellerID) {
    string query = "SELECT DATE(OrderDate) AS Date, SUM(TotalAmount) AS DailySales FROM orders WHERE UserID = " + sellerID + " GROUP BY DATE(OrderDate) ORDER BY Date";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << "========== SALES TRENDS ==========\n";
        cout << left << setw(15) << "Date" << setw(15) << "Daily Sales" << endl;
        cout << string(30, '-') << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(15) << row[0] << setw(15) << row[1] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void updateCommissionRateInterface() {
    while (true) {
        clearScreen();
        double newRate;

        cout << "========== UPDATE COMMISSION RATE ==========\n";
        cout << "[1] Enter new commission rate\n";
        cout << "[2] Back to Admin Menu\n";
        cout << "============================================\n";
        cout << "Select an option: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            cout << "Enter new commission rate (in %): ";
            cin >> newRate;

            if (newRate < 0 || newRate > 100) {
                cout << "Invalid rate. Please enter a value between 0 and 100." << endl;
            }
            else {
                string query = "UPDATE system_settings SET Value = " + to_string(newRate / 100) + " WHERE SettingName = 'CommissionRate'";
                if (!executeUpdate(query)) {
                    cerr << "Error updating commission rate: " << mysql_error(conn) << endl;
                }
                else {
                    cout << "Commission rate updated successfully!" << endl;
                }
            }
        }
        else if (choice == 2) {
            return;  // Back to Admin Menu
        }
        else {
            cout << "Invalid option. Please try again." << endl;
        }

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

void viewPlatformSalesReportsInterface() {
    while (true) {
        clearScreen();
        viewPlatformSalesReports();

        cout << "\nPress Enter to return to the Admin Menu...";
        cin.ignore();
        cin.get();
        return;
    }
}

void generateHTMLReportInterface() {
    while (true) {
        clearScreen();
        generateHTMLReport();

        cout << "\nPress Enter to return to the Admin Menu...";
        cin.ignore();
        cin.get();
        return;
    }
}

void manageProfileInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "================== MY PROFILE ===================\n";
        manageProfile();
        cout << "=================================================\n";
        cout << "\nPress Enter to return to the Seller Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Menu
    }
}

void manageChatsWithCustomersInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "================== VIEW MESSAGES ===================\n";
        cout << "========== CHAT WITH CUSTOMERS ==========\n";
        listCustomersForChat(stoi(glbStr)); // List customers the seller has interacted with

        int customerID;
        cout << "\nEnter Customer ID to view messages (or 0 to go back): ";
        cin >> customerID;

        if (customerID == 0) {
            return; // Back to Seller Menu
        }

        // Display chat history
        viewMessages(stoi(glbStr), customerID);

        string reply;
        cout << "Enter your reply: ";
        cin.ignore(); // Clear the newline character from the input buffer
        getline(cin, reply);

        sendMessage(stoi(glbStr), customerID, reply); // Send reply from seller to customer
        viewMessages(stoi(glbStr), customerID); // View updated chat history with the customer

        cout << "\nPress Enter to return to the Seller Menu...";
        cin.ignore();
        cin.get();
    }
}

void approveCustomerOrdersInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "================= APPROVE CUSTOMER ORDERS =================\n";
        approveCustomerOrders();
        cout << "===========================================================\n";
        cout << "================= UPDATE ORDER STATUS =====================\n";
        string orderID, statusID;
        cout << "Enter Order ID: ";
        cin >> orderID;
        cout << "Enter New Status ID: ";
        cin >> statusID;
        updateOrderStatus(orderID, statusID);
        cout << "==========================================================\n";
        cout << "\nPress Enter to return to the Seller Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Menu
    }
}

void viewWalletBalanceInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "=================== VIEW WALLET BALANCE ==================\n";
        viewWalletBalance();
        cout << "==========================================================\n";
        cout << "\nPress Enter to return to the Seller Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Menu
    }
}

void sellerSalesReportInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "=================== SELLER SALES REPORT ===================\n";
        cout << "| [1] Generate Sales Report                               |\n";
        cout << "| [2] View Customer Demographics                          |\n";
        cout << "| [3] View Product Popularity                             |\n";
        cout << "| [4] Back to Seller Menu                                 |\n";
        cout << "===========================================================\n";
        int choice;
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            generateSalesReportInterface();
            break;
        case 2:
            viewCustomerDemographicsInterface();
            break;
        case 3:
            viewProductPopularityInterface();
            break;
        case 4:
            return; // Back to Seller Menu
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

void generateSalesReportInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "======================= GENERATE SALES REPORT =======================\n";

        // Correct SQL query
        string query = "SELECT SUM(oi.UnitPrice * oi.Quantity) AS TotalSales, COUNT(o.OrderID) AS OrderCount "
            "FROM orders o INNER JOIN order_items oi ON o.OrderID = oi.OrderID "
            "WHERE o.UserID = " + glbStr;
        MYSQL_RES* res = executeSelectQuery(query);

        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {
                cout << "Total Sales: $" << fixedPrice(stod(row[0])) << endl;
                cout << "Total Orders: " << row[1] << endl;
            }
            else {
                cout << "No sales data found for your account." << endl;
            }
            mysql_free_result(res);
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        cout << "====================================================================\n";
        cout << "\nPress Enter to return to Seller Sales Report Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Sales Report Menu
    }
}

void viewCustomerDemographicsInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "================== VIEW CUSTOMER DEMOGRAPHICS ===================\n";

        // Correct SQL query
        string query = "SELECT u.City, COUNT(o.OrderID) AS OrderCount "
            "FROM orders o INNER JOIN user u ON o.UserID = u.UserID "
            "INNER JOIN order_items oi ON o.OrderID = oi.OrderID "
            "WHERE oi.SellerID = " + glbStr + " GROUP BY u.City";
        MYSQL_RES* res = executeSelectQuery(query);

        if (res) {
            MYSQL_ROW row;
            cout << left << setw(20) << "City" << setw(20) << "Order Count" << endl;
            cout << string(40, '-') << endl;

            while ((row = mysql_fetch_row(res))) {
                cout << left << setw(20) << row[0] << setw(20) << row[1] << endl;
            }
            mysql_free_result(res);
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        cout << "==================================================================\n";
        cout << "\nPress Enter to return to Seller Sales Report Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Sales Report Menu
    }
}

void viewProductPopularityInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "================== VIEW PRODUCT POPULARITY ===================\n";

        // Correct SQL query
        string query = "SELECT p.ProductName, SUM(oi.Quantity) AS TotalQuantity "
            "FROM order_items oi INNER JOIN product p ON oi.ProductID = p.ProductID "
            "WHERE oi.SellerID = " + glbStr + " GROUP BY p.ProductName";
        MYSQL_RES* res = executeSelectQuery(query);

        if (res) {
            MYSQL_ROW row;
            cout << left << setw(30) << "Product Name" << setw(20) << "Total Quantity" << endl;
            cout << string(50, '-') << endl;

            while ((row = mysql_fetch_row(res))) {
                cout << left << setw(30) << row[0] << setw(20) << row[1] << endl;
            }
            mysql_free_result(res);
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        cout << "==============================================================\n";
        cout << "\nPress Enter to return to Seller Sales Report Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Sales Report Menu
    }
}

void viewTransactionHistoryInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "=============== VIEW TRANSACTION HISTORY =================\n";
        viewTransactionHistory();
        cout << "==========================================================\n";
        cout << "\nPress Enter to return to the Seller Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Menu
    }
}

void viewSalesReportsInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== VIEW SALES REPORTS ==================\n";
        viewSalesReports();
        cout << "==========================================================\n";
        cout << "\nPress Enter to return to the Seller Menu...";
        cin.ignore();
        cin.get();
        return; // Back to Seller Menu
    }
}

void Logout() {
    // Option to confirm to go back to the main menu
    int ConfirmChoice;
    cout << "================ LOG OUT SYSTEM ? ===============\n";
    cout << "[1] Log Out Account\n";
    cout << "[2] Cancel Action\n";
    cout << "=================================================\n";
    cout << "Select an option: ";
    cin >> ConfirmChoice;

    if (ConfirmChoice == 1) {
        mainMenu(); // Go back to the main menu
    }
    else if (ConfirmChoice != 2) {
        return;
    }
}

void viewAndUpdateProfile(const string& userID) {
    clearScreen();
    string query = "SELECT Username, Email, FullName, Address, PhoneNumber FROM user WHERE UserID = " + userID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            // Display profile details with an improved look
            cout << "=============================================================\n";
            cout << "|                         PROFILE DETAILS                    |\n";
            cout << "=============================================================\n";
            cout << "| Username       : " << left << setw(40) << row[0] << "|\n";
            cout << "| Email          : " << left << setw(40) << row[1] << "|\n";
            cout << "| Full Name      : " << left << setw(40) << row[2] << "|\n";
            cout << "| Address        : " << left << setw(40) << row[3] << "|\n";
            cout << "| Phone Number   : " << left << setw(40) << row[4] << "|\n";
            cout << "=============================================================\n";
        }
        else {
            cout << "No profile found for the provided UserID.\n";
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    cout << "\nDo you want to update your profile? (y/n): ";
    char updateChoice;
    cin >> updateChoice;

    if (tolower(updateChoice) == 'y') {
        cout << "=============================================================\n";
        cout << "|                      UPDATE PROFILE                        |\n";
        cout << "=============================================================\n";

        string username, email, password, fullName, address, phoneNumber;

        cout << "| Enter new Username     : ";
        cin.ignore(); // To ignore any leftover newline character from previous input
        getline(cin, username);

        cout << "| Enter new Email        : ";
        getline(cin, email);

        cout << "| Enter new Password     : ";
        getline(cin, password);
        password = bcrypt::generateHash(password); // Hash the password

        cout << "| Enter new Full Name    : ";
        getline(cin, fullName);

        cout << "| Enter new Address      : ";
        getline(cin, address);

        cout << "| Enter new Phone Number : ";
        getline(cin, phoneNumber);

        string query = "UPDATE user SET Username = '" + username + "', Email = '" + email + "', Password = '" + password + "', FullName = '" + fullName + "', Address = '" + address + "', PhoneNumber = '" + phoneNumber + "' WHERE UserID = " + userID;
        if (!executeUpdate(query)) {
            cout << "Error: " << mysql_error(conn) << endl;
        }
        else {
            cout << "Profile updated successfully!\n";
        }
    }

    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewProfile() {
    clearScreen();
    displayBanner();
    cout << "==================== VIEW PROFILE ====================\n";
    string query = "SELECT * FROM user WHERE UserID = " + currentUserID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "User ID: " << row[0] << endl;
            cout << "Username: " << row[1] << endl;
            cout << "Email: " << row[2] << endl;
            cout << "Full Name: " << row[4] << endl;
            cout << "Address: " << row[5] << endl;
            cout << "Phone Number: " << row[6] << endl;
        }
        else {
            cout << "Profile not found." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "=====================================================\n";
    cout << "\nPress Enter to return to the My Profile Menu...";
    cin.ignore();
    cin.get();
}

void editProfile() {
    clearScreen();
    displayBanner();
    cout << "==================== EDIT PROFILE ====================\n";
    string query = "SELECT * FROM user WHERE UserID = " + currentUserID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            string email, fullName, address, phoneNumber;
            cout << "Enter new email (current: " << row[2] << "): ";
            cin >> email;
            cout << "Enter new full name (current: " << row[4] << "): ";
            cin.ignore();
            getline(cin, fullName);
            cout << "Enter new address (current: " << row[5] << "): ";
            getline(cin, address);
            cout << "Enter new phone number (current: " << row[6] << "): ";
            getline(cin, phoneNumber);

            string updateQuery = "UPDATE user SET Email = '" + email + "', FullName = '" + fullName + "', Address = '" + address + "', PhoneNumber = '" + phoneNumber + "' WHERE UserID = " + currentUserID;
            if (executeUpdate(updateQuery)) {
                cout << "Profile updated successfully!" << endl;
            }
            else {
                cout << "Error updating profile: " << mysql_error(conn) << endl;
            }
        }
        else {
            cout << "Profile not found." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "=====================================================\n";
    cout << "\nPress Enter to return to the My Profile Menu...";
    cin.ignore();
    cin.get();
}

void myWalletInterface() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== MY WALLET ====================\n";
        cout << "| [1] Create E-Wallet Account                      |\n";
        cout << "| [2] Top-Up Wallet                                |\n";
        cout << "| [3] View Wallet Balance                          |\n";
        cout << "| [4] Back to Customer Menu                        |\n";
        cout << "=====================================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            createEWalletAccount();
            break;
        case 2:
            topUpWallet();
            break;
        case 3:
            viewWalletBalance();
            break;
        case 4:
            return; // Back to Customer Menu
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

void viewWalletBalance() {
    clearScreen();
    displayBanner();
    cout << "==================== VIEW WALLET BALANCE ====================\n";
    string query = "SELECT Balance FROM wallet WHERE UserID = " + currentUserID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "| Wallet Balance: $" << fixed << setprecision(2) << stod(row[0]) << endl;
        }
        else {
            cout << "| Wallet not found." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "===========================================================\n";
    cout << "\nPress Enter to return to the My Wallet Menu...";
    cin.ignore();
    cin.get();
}

void createEWalletAccount() {
    clearScreen();
    displayBanner();
    cout << "==================== CREATE E-WALLET ACCOUNT ====================\n";

    string initialBalance;
    cout << "| Enter initial balance: $";
    cin.ignore();
    getline(cin, initialBalance);

    string query = "INSERT INTO wallet (UserID, Balance) VALUES (" + currentUserID + ", " + initialBalance + ")";
    if (executeUpdate(query)) {
        cout << "E-Wallet account created successfully with an initial balance of $" << initialBalance << "!\n";
    }
    else {
        cout << "Error creating E-Wallet account: " << mysql_error(conn) << endl;
    }

    cout << "===============================================================\n";
    cout << "\nPress Enter to return to the My Wallet Menu...";
    cin.ignore();
    cin.get();
}

void topUpWallet() {
    clearScreen();
    displayBanner();
    cout << "==================== TOP-UP WALLET ====================\n";
    double amount;
    cout << "| Enter amount to top-up: $";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount. Please enter a positive value." << endl;
    }
    else {
        string query = "UPDATE wallet SET Balance = Balance + " + to_string(amount) + " WHERE UserID = " + currentUserID;
        if (executeUpdate(query)) {
            cout << "Wallet topped up successfully! New balance will be reflected soon." << endl;
        }
        else {
            cout << "Error topping up wallet: " << mysql_error(conn) << endl;
        }
    }

    cout << "=======================================================\n";
    cout << "\nPress Enter to return to the My Wallet Menu...";
    cin.ignore();
    cin.get();
}   

void messagePageInterface() {
    clearScreen();
    displayBanner();

    // Query to get the messages for the current user
    string query = "SELECT u.UserID, u.StoreName, m.SenderID, m.ReceiverID, m.MessageText, m.Timestamp "
        "FROM messages m "
        "INNER JOIN user u ON (m.SenderID = u.UserID OR m.ReceiverID = u.UserID) "
        "WHERE m.SenderID = " + currentUserID + " OR m.ReceiverID = " + currentUserID + " "
        "ORDER BY m.Timestamp DESC";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        if (mysql_num_rows(res) == 0) {
            // No messages found
            cout << "=============================================================\n";
            cout << "|                         MESSAGES                           |\n";
            cout << "=============================================================\n";
            cout << "\nTHERE'S NO MESSAGES, WOULD YOU LIKE TO START A NEW ONE?\n";
            cout << "=============================================================\n";
            cout << "[1] START A NEW MESSAGE\n";
            cout << "[2] RETURN BACK\n";
            cout << "\nYOUR OPTION: ";

            int option;
            cin >> option;
            if (option == 1) {
                startNewMessage();
            }
            else {
                return;
            }
        }
        else {
            // Messages found
            cout << "=============================================================\n";
            cout << "|                         MESSAGES                           |\n";
            cout << "=============================================================\n";

            set<int> displayedSellers;

            while ((row = mysql_fetch_row(res))) {
                int sellerID = stoi(row[0]);
                string storeName = (row[1] && strlen(row[1]) > 0) ? row[1] : "Unknown Store";

                // Only display the latest message from each seller
                if (displayedSellers.find(sellerID) == displayedSellers.end()) {
                    cout << "- [" << storeName << "]  [" << sellerID << "]\n";
                    if (stoi(row[2]) == stoi(currentUserID)) {
                        cout << "You: " << row[4] << " [" << row[5] << "]\n\n";
                    }
                    else {
                        cout << "Seller: " << row[4] << " [" << row[5] << "]\n\n";
                    }
                    displayedSellers.insert(sellerID);
                }
            }

            cout << "=============================================================\n";
            cout << "[1] START A NEW MESSAGE\n";
            cout << "[2] SELECT YOUR MESSAGE\n";
            cout << "[3] RETURN BACK\n";
            cout << "\nYOUR OPTION: ";

            int option;
            cin >> option;
            if (option == 1) {
                startNewMessage();
            }
            else if (option == 2) {
                selectMessage();
            }
            else {
                return;
            }
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "\nPress Enter to return to the Message Page...";
    cin.ignore();
    cin.get();
}

void startNewMessage() {
    // Logic to start a new message
    clearScreen();
    displayBanner();

    // Query to get the list of sellers
    string query = "SELECT UserID, StoreName FROM user WHERE UserRole = 1 AND isApproved = 1";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << "=============================================================\n";
        cout << "|                         SELLERS                            |\n";
        cout << "=============================================================\n";

        while ((row = mysql_fetch_row(res))) {
            int sellerID = stoi(row[0]);
            string storeName = row[1] ? row[1] : "Unknown Store";
            cout << "- [" << storeName << "]  [" << sellerID << "]\n";
        }

        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    cout << "=============================================================\n";
    cout << "Enter Seller ID to start a new message: ";
    int sellerID;
    cin >> sellerID;
    cin.ignore(); // Clear the newline character from the input buffer

    cout << "\n=============================================================\n";
    cout << "|                         NEW MESSAGE                        |\n";
    cout << "=============================================================\n";
    string message;
    cout << "Enter your message: ";
    getline(cin, message);

    sendMessage(stoi(currentUserID), sellerID, message); // Send message from customer to seller

    cout << "\n=============================================================\n";
    cout << "Message sent successfully!\n";
    cout << "=============================================================\n";

    viewMessages(stoi(currentUserID), sellerID); // View updated chat history with the seller

    cout << "\nPress Enter to return to the Message Page...";
    cin.ignore(); // This line is now unnecessary since cin.ignore() was called earlier
    cin.get();
}

void selectMessage() {
    // Logic to select and view existing messages
    clearScreen();
    displayBanner();

    // Query to get the list of sellers
    string query = "SELECT UserID, StoreName FROM user WHERE UserRole = 1 AND isApproved = 1";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << "=============================================================\n";
        cout << "|                         SELLERS                            |\n";
        cout << "=============================================================\n";

        while ((row = mysql_fetch_row(res))) {
            int sellerID = stoi(row[0]);
            string storeName = row[1] ? row[1] : "Unknown Store";
            cout << "- [" << storeName << "]  [" << sellerID << "]\n";
        }

        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    cout << "=============================================================\n";
    cout << "Enter Seller ID to view messages: ";
    int sellerID;
    cin >> sellerID;
    cin.ignore(); // Clear the newline character from the input buffer

    viewMessages(stoi(currentUserID), sellerID); // Display chat history with the selected seller

    cout << "\n=============================================================\n";
    cout << "|                         REPLY                              |\n";
    cout << "=============================================================\n";
    string reply;
    cout << "Enter your reply: ";
    getline(cin, reply);

    sendMessage(stoi(currentUserID), sellerID, reply); // Send message from customer to seller
    viewMessages(stoi(currentUserID), sellerID); // View updated chat history with the seller

    cout << "\n=============================================================\n";
    cout << "Message sent successfully!\n";
    cout << "=============================================================\n";

    cout << "\nPress Enter to return to the Message Page...";
    cin.ignore();
    cin.get();
}

void browseProductsInterface() {
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== BROWSE PRODUCTS ====================\n";
        string query = "SELECT * FROM product";
        MYSQL_RES* res = executeSelectQuery(query);

        if (res) {
            MYSQL_ROW row;
            cout << left << setw(10) << "Product ID" << setw(30) << "Name" << setw(20) << "Price" << setw(10) << "Stock" << endl;
            cout << string(70, '-') << endl;

            while ((row = mysql_fetch_row(res))) {
                cout << left << setw(10) << row[0] << setw(30) << row[1] << setw(20) << fixed << setprecision(2) << stod(row[3]) << setw(10) << row[4] << endl;
            }
            mysql_free_result(res);
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        int productID;
        cout << "\nEnter Product ID to view details (or 0 to go back): ";
        cin >> productID;

        if (productID == 0) {
            return; // Back to Customer Menu
        }

        viewProductDetails(productID);

        cout << "\nPress Enter to return to the Browse Products Menu...";
        cin.ignore();
        cin.get();
    }
}

void viewProductDetails(int productId) {
    clearScreen();
    displayBanner();
    cout << "==================== PRODUCT DETAILS ====================\n";
    string query = "SELECT * FROM product WHERE ProductID = " + to_string(productId);
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "Product ID: " << row[0] << endl;
            cout << "Name: " << row[1] << endl;
            cout << "Description: " << row[2] << endl;
            cout << "Price: $" << fixed << setprecision(2) << stod(row[3]) << endl;
            cout << "Stock Quantity: " << row[4] << endl;
            cout << "Store Name: " << row[7] << endl;
        }
        else {
            cout << "Product not found." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "=========================================================\n";
    cout << "\nPress Enter to return to the Browse Products Menu...";
    cin.ignore();
    cin.get();
}

void myCartInterface() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== MY CART ====================\n";
        cout << "| [1] View Cart                                  |\n";
        cout << "| [2] Apply Coupon                               |\n";
        cout << "| [3] Back to Customer Menu                      |\n";
        cout << "=================================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            viewCart();
            break;
        case 2:
            applyCoupon();
            break;
        case 3:
            return; // Back to Customer Menu
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
}

void viewCart() {
    clearScreen();
    displayBanner();
    cout << "==================== VIEW CART ====================\n";
    if (cart.empty()) {
        cout << "Your cart is empty." << endl;
    }
    else {
        cout << left << setw(10) << "Product ID" << setw(30) << "Product Name" << setw(20) << "Store Name" << setw(10) << "Quantity" << setw(10) << "Price" << endl;
        cout << string(80, '-') << endl;

        for (const auto& item : cart) {
            cout << left << setw(10) << item.productId << setw(30) << item.productName << setw(20) << item.storeName << setw(10) << item.quantity << setw(10) << fixed << setprecision(2) << item.unitPrice << endl;
        }
        cout << "=====================================================\n";
        cout << "Total Amount: $" << fixed << setprecision(2) << totalAmount << endl;
    }

    cout << "\nPress Enter to return to the My Cart Menu...";
    cin.ignore();
    cin.get();
}

void applyCoupon() {
    clearScreen();
    displayBanner();
    cout << "==================== APPLY COUPON ====================\n";
    string couponCode;
    cout << "Enter coupon code: ";
    cin >> couponCode;

    // Example coupon application logic
    if (couponCode == "DISCOUNT10") {
        double discount = totalAmount * 0.10;
        totalAmount -= discount;
        cout << "Coupon applied! You saved $" << fixed << setprecision(2) << discount << ". New total amount: $" << totalAmount << endl;
    }
    else {
        cout << "Invalid coupon code." << endl;
    }

    cout << "=====================================================\n";
    cout << "\nPress Enter to return to the My Cart Menu...";
    cin.ignore();
    cin.get();
}

void checkoutInterface() {
    clearScreen();
    displayBanner();
    cout << "==================== CHECKOUT ====================\n";
    if (cart.empty()) {
        cout << "Your cart is empty. Please add products to the cart before checking out." << endl;
    }
    else {
        cout << "Proceeding to checkout..." << endl;
        // Generate a new order and save to the database
        string query = "INSERT INTO orders (UserID, TotalAmount, Status, OrderDate) VALUES (" + currentUserID + ", " + to_string(totalAmount) + ", 'Pending', NOW())";

        if (executeUpdate(query)) {
            // Retrieve the last inserted order ID
            query = "SELECT LAST_INSERT_ID()";
            MYSQL_RES* res = executeSelectQuery(query);
            MYSQL_ROW row = mysql_fetch_row(res);
            int orderId = stoi(row[0]);
            mysql_free_result(res);

            for (const auto& item : cart) {
                string itemQuery = "INSERT INTO order_items (OrderID, ProductID, Quantity, UnitPrice) VALUES (" + to_string(orderId) + ", " + to_string(item.productId) + ", " + to_string(item.quantity) + ", " + to_string(item.unitPrice) + ")";
                executeUpdate(itemQuery);
            }

            // Clear the cart
            cart.clear();
            totalAmount = 0;

            cout << "Order placed successfully! Your order ID is " << orderId << "." << endl;
            cout << "Proceed to Payment Transaction" << endl;
            paymentTransaction(orderId);
        }
        else {
            cout << "Error placing order: " << mysql_error(conn) << endl;
        }

        cout << "\nPress Enter to return to the Customer Menu...";
        cin.ignore();
        cin.get();
    }
}

void paymentTransaction(int orderId) {
    clearScreen();
    displayBanner();
    cout << "==================== PAYMENT TRANSACTION ====================\n";
    string query = "SELECT Balance FROM wallet WHERE UserID = " + currentUserID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            double walletBalance = stod(row[0]);
            cout << "Wallet Balance: $" << fixed << setprecision(2) << walletBalance << endl;
            cout << "Order Total: $" << fixed << setprecision(2) << totalAmount << endl;

            if (walletBalance >= totalAmount) {
                cout << "Confirm payment? (yes/no): ";
                string confirm;
                cin >> confirm;
                if (confirm == "yes") {
                    // Deduct amount from wallet and update order status
                    string updateWallet = "UPDATE wallet SET Balance = Balance - " + to_string(totalAmount) + " WHERE UserID = " + currentUserID;
                    executeUpdate(updateWallet);

                    string updateOrder = "UPDATE orders SET Status = 'Completed' WHERE OrderID = " + to_string(orderId);
                    executeUpdate(updateOrder);

                    cout << "Payment successful! Order completed." << endl;
                    generateReceipt(orderId);
                }
                else {
                    cout << "Payment canceled." << endl;
                }
            }
            else {
                cout << "Insufficient wallet balance. Please top-up your wallet." << endl;
            }
        }
        else {
            cout << "Wallet not found." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "===========================================================\n";
    cout << "\nPress Enter to return to the Checkout Menu...";
    cin.ignore();
    cin.get();
}

void generateReceipt(int orderId) {
    clearScreen();
    displayBanner();
    cout << "==================== RECEIPT ====================\n";
    string query = "SELECT p.ProductName, oi.Quantity, oi.UnitPrice FROM order_items oi INNER JOIN product p ON oi.ProductID = p.ProductID WHERE oi.OrderID = " + to_string(orderId);
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        double totalAmount = 0.0;
        cout << left << setw(30) << "Product Name" << setw(10) << "Quantity" << setw(10) << "Price" << endl;
        cout << string(50, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            string productName = row[0];
            int quantity = stoi(row[1]);
            double unitPrice = stod(row[2]);
            double itemTotal = unitPrice * quantity;

            cout << left << setw(30) << productName << setw(10) << quantity << setw(10) << fixed << setprecision(2) << unitPrice << " $" << fixed << setprecision(2) << itemTotal << endl;
            totalAmount += itemTotal;
        }

        cout << string(50, '-') << endl;
        cout << left << setw(30) << "Total Amount" << setw(10) << " " << setw(10) << " " << " $" << fixed << setprecision(2) << totalAmount << endl;

        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "=================================================\n";
    cout << "\nPress Enter to return to the Customer Menu...";
    cin.ignore();
    cin.get();
}

void viewOrderHistoryInterface() {
    clearScreen();
    displayBanner();
    cout << "==================== VIEW ORDER HISTORY ====================\n";
    string query = "SELECT OrderID, TotalAmount, Status, OrderDate FROM orders WHERE UserID = " + currentUserID + " ORDER BY OrderDate DESC";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "Order ID" << setw(20) << "Total Amount" << setw(20) << "Status" << setw(20) << "Order Date" << endl;
        cout << string(70, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << fixed << setprecision(2) << stod(row[1]) << setw(20) << row[2] << setw(20) << row[3] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "========================================================\n";
    cout << "\nPress Enter to return to the Customer Menu...";
    cin.ignore();
    cin.get();
}

void listSellers() {
    clearScreen();
    displayBanner();
    cout << "==================== LIST OF SELLERS ====================\n";

    string query = "SELECT UserID, StoreName FROM user WHERE UserRole = 1 AND isApproved = 1";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "Seller ID" << setw(30) << "Store Name" << endl;
        cout << string(40, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(30) << row[1] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "========================================================\n";
    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

void myOrderInterface() {
    clearScreen();
    displayBanner();
    cout << "==================== MY ORDERS ====================\n";

    string query = "SELECT OrderID, TotalAmount, OrderStatus, OrderDate FROM orders WHERE UserID = " + currentUserID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        cout << left << setw(10) << "Order ID" << setw(20) << "Total Amount" << setw(20) << "Status" << setw(20) << "Order Date" << endl;
        cout << string(70, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << fixed << setprecision(2) << stod(row[1]) << setw(20) << row[2] << setw(20) << row[3] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "========================================================\n";
    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

int main() {
    connectToDatabase(); // Establish database connection
    mainMenu(); // Start the main menu
    return 0;
}
