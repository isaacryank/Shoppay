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
    string productID;
    int quantity;
    double unitPrice;
    string productName; // Added product name for easy reference
    string storeName; // Added store name for easy reference
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
void viewPlatformSalesReports();
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
void eWalletManagement();
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

void viewMessages(int userID1, int userID2);
void sendMessage(int senderID, int receiverID, const std::string& messageText);
void updateProfile(const string& userID);
void viewProfile(const string& userID);
void updateProfile(const string& userID);

void addProduct();
void updateProduct();
void deleteProduct(); 
void searchProducts();
void sortProducts();
void searchProducts();
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
    cout << "           WELCOME TO SHOPPAY SYSTEM                 \n";
    cout << "   A PLACE WITH PEACE OF MIND TO SHOPPING            \n";
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
        cerr << "Error executing update: " << mysql_error(conn) << "\n";
        cerr << "Query: " << query << "\n"; // Print the query for debugging
        return false;
    }
    return true;
}

MYSQL_RES* executeSelectQuery(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing query: " << mysql_error(conn) << "\n";
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

    cout << "Enter Username: ";
    cin.ignore(); // To ignore any leftover newline character from previous input
    getline(cin, newUser.username);

    cout << "Enter Email: ";
    getline(cin, newUser.email);

    cout << "Enter Password: ";
    getline(cin, newUser.password);

    cout << "Enter Full Name: ";
    getline(cin, newUser.fullName);

    cout << "Enter Phone Number: ";
    getline(cin, newUser.phoneNumber);

    newUser.isApproved = false; // Default to not approved

    cout << "Select User Role:\n[1] Admin\n[2] Seller\n[3] Customer\n";
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
        // Debugging statements
        cout << "Debug Info: Username = " << newUser.username << endl;
        cout << "Debug Info: Email = " << newUser.email << endl;
        cout << "Debug Info: Password = " << newUser.password << endl;
        cout << "Debug Info: Full Name = " << newUser.fullName << endl;
        cout << "Debug Info: Phone Number = " << newUser.phoneNumber << endl;
        cout << "Debug Info: User Role = " << newUser.UserRole << endl;

        // Insert user into the database
        string query = "INSERT INTO user (Username, Email, Password, FullName, PhoneNumber, UserRole, isApproved, StoreName) VALUES ('" +
            newUser.username + "', '" + newUser.email + "', '" + newUser.password + "', '" +
            newUser.fullName + "', '" + newUser.phoneNumber + "', " +
            to_string(newUser.UserRole) + ", " + to_string(newUser.isApproved) + ", '" + newUser.storeName + "')";
        if (!executeUpdate(query)) {
            cout << "Error: " << mysql_error(conn) << endl;
        }
        else {
            cout << "Registration successful!\n";
        }
    }
    else {
        cout << "Registration was not completed.\n";
    }
}

// Sign In Account
void signInAccount() {
    string username, password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    string query = "SELECT UserID, UserRole, isApproved FROM user WHERE Username = '" + username + "' AND Password = '" + password + "'";
    MYSQL_RES* res = executeSelectQuery(query);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            int userID = atoi(row[0]);
            int userRole = atoi(row[1]);
            int isApproved = atoi(row[2]);

            glbStr = to_string(userID); // Store UserID in global variable
            if (userRole == 2) {
                adminMenu();
            }
            else if (userRole == 1 && isApproved) {
                sellerMenu();
            }
            else if (userRole == 0) {
                customerMenu();
            }
            else {
                cout << "Login failed: Your account is not approved yet.\n";
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
}

// Admin Menu
// Admin Menu
void adminMenu() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "[1] Manage Products\n";
        cout << "[2] Approve Registered Sellers\n";
        cout << "[3] Coupon Management\n";
        cout << "[4] Monitor All Wallet Transactions\n";
        cout << "[5] Update Commission Rate\n";
        cout << "[6] View Platform Sales Reports\n";
        cout << "[7] Generate Graph Reports\n"; // New Option
        cout << "[8] Logout\n";
        cout << "Select an option: ";
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
            updateCommissionRate();
            break;
        case 6:
            viewPlatformSalesReports();
            break;
        case 7:
            generateHTMLReport(); // Call the new function
            break;
        case 8:
            return;
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
        cout << "[1] Manage My Products\n";
        cout << "[2] Browse Products\n";
        cout << "[3] Approve Customer Orders\n";
        cout << "[4] View Wallet Balance\n";
        cout << "[5] View Sales Reports\n";
        cout << "[6] Transaction History\n";
        cout << "[7] Update Order Status\n";
        cout << "[8] Generate Sales Report\n";
        cout << "[9] View Customer Demographics\n";
        cout << "[10] View Product Popularity\n";
        cout << "[11] View Messages\n";
        cout << "[12] My Profile\n"; // New option
        cout << "[13] Logout\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            manageMyProducts();
            break;
        case 2:
            browseProducts();
            break;
        case 3:
            approveCustomerOrders();
            break;
        case 4:
            viewWalletBalance();
            break;
        case 5:
            viewSalesReports();
            break;
        case 6:
            viewTransactionHistory();
            break;
        case 7:
        {
            string orderID, statusID;
            cout << "Enter Order ID: ";
            cin >> orderID;
            cout << "Enter New Status ID: ";
            cin >> statusID;
            updateOrderStatus(orderID, statusID);
        }
        break;
        case 8:
            generateSalesReport(glbStr);
            break;
        case 9:
            viewCustomerDemographics(glbStr);
            break;
        case 10:
            viewProductPopularity(glbStr);
            break;
        case 11:
            manageChatsWithCustomers();
            break;
        case 12: // New option
        {
            viewProfile(glbStr);
            cout << "Do you want to update your profile? (y/n): ";
            char updateChoice;
            cin >> updateChoice;
            if (tolower(updateChoice) == 'y') {
                updateProfile(glbStr);
            }
        }
        break;
        case 13:
            return;
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
        cout << "[1] Browse Products\n";
        cout << "[2] View Cart\n";
        cout << "[3] Apply Coupon\n";
        cout << "[4] Checkout\n";
        cout << "[5] E-Wallet Management\n";
        cout << "[6] View Order Status\n";
        cout << "[7] View Transaction History\n";
        cout << "[8] View Receipt\n";
        cout << "[9] My Profile\n"; // New option
        cout << "[10] Logout\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            browseProducts();
            break;
        case 2:
            viewCartWithChatOption();
            break;
        case 3:
            applyCoupon();
            break;
        case 4:
            checkout();
            break;
        case 5:
            eWalletManagement();
            break;
        case 6:
            viewOrderStatus(glbStr);
            break;
        case 7:
            viewTransactionHistory();
            break;
        case 8:
            viewReceipt();
            break;
        case 9: // New option
        {
            viewProfile(glbStr);
            cout << "Do you want to update your profile? (y/n): ";
            char updateChoice;
            cin >> updateChoice;
            if (tolower(updateChoice) == 'y') {
                updateProfile(glbStr);
            }
        }
        break;
        case 10:
            return;
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
        cout << "[7] Back to Admin Menu\n";
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
            return;
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
    string keyword;

    cout << "========== SEARCH PRODUCTS ==========\n";
    cout << "Enter keyword: ";
    cin.ignore();
    getline(cin, keyword);

    string query = "SELECT * FROM product WHERE ProductName LIKE '%" + keyword + "%'";
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
        cout << left << setw(10) << "UserID" << setw(20) << "Username" << setw(30) << "Email"
            << setw(20) << "Full Name" << setw(15) << "Phone Number" << setw(20) << "Registration Date" << endl;
        cout << string(120, '=') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(30) << row[2]
                << setw(20) << row[3] << setw(15) << row[4] << setw(20) << row[5] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    int userID;
    char decision;
    cout << "Enter UserID to approve or decline (or '0' to go back): ";
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
        cout << (tolower(decision) == 'y' ? "Seller approved!" : "Seller declined and deleted!") << endl;
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
    clearScreen();
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

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
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

    string query = "UPDATE system_settings SET CommissionRate = " + to_string(newRate / 100) + " WHERE SettingName = 'CommissionRate'";
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

void viewCart() {
    clearScreen();
    cout << "========== VIEW CART ==========\n";
    if (cart.empty()) {
        cout << "Your cart is empty.\n";
    }
    else {
        for (const auto& item : cart) {
            cout << "Product ID: " << item.productID
                << ", Product Name: " << item.productName
                << ", Quantity: " << item.quantity
                << ", Unit Price: $" << fixedPrice(item.unitPrice)
                << ", Store: " << item.storeName << endl;
        }
        cout << "Total Amount: $" << fixedPrice(totalAmount) << endl;
    }
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void applyCoupon() {
    string code;
    cout << "Enter Coupon Code: ";
    cin >> code;

    string query = "SELECT DiscountPercentage FROM coupon WHERE Code = '" + code + "'";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            double discount = stod(row[0]);
            totalAmount -= (totalAmount * (discount / 100));
            cout << "Coupon applied! New Total Amount: $" << totalAmount << endl;
        }
        else {
            cout << "Invalid coupon code.\n";
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

void viewWalletBalance() {
    clearScreen();
    string query = "SELECT Balance FROM wallet WHERE UserID = " + glbStr;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "Your Wallet Balance: $" << fixedPrice(stod(row[0])) << endl;
        }
        else {
            cout << "No wallet found for your account." << endl;
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
            cout << "HTML report generated successfully.\n";
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

void topUpWallet() {
    double amount;
    cout << "Enter the amount to top-up: $";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount. Please enter a positive value.\n";
        return;
    }

    // Update the wallet balance in the database
    string query = "UPDATE wallet SET Balance = Balance + " + to_string(amount) + " WHERE UserID = " + glbStr;
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        // Insert a transaction record for the top-up
        string transactionQuery = "INSERT INTO wallet_transaction (WalletID, Amount, TransactionType) VALUES ((SELECT WalletID FROM wallet WHERE UserID = " + glbStr + "), " + to_string(amount) + ", 'Top-Up')";
        if (!executeUpdate(transactionQuery)) {
            cout << "Error: " << mysql_error(conn) << endl;
        }
        else {
            cout << "Top-up successful! Your wallet has been credited with $" << fixedPrice(amount) << ".\n";
        }
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
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
            return;
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

    // Execute the query
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Error: " << mysql_error(conn) << std::endl;
        return;
    }

    // Store the result set
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        std::cerr << "Error: " << mysql_error(conn) << std::endl;
        return;
    }

    // Process the result set
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        std::cout << (atoi(row[0]) == userID1 ? "You: " : "Them: ") << row[1] << " [" << row[2] << "]\n";
    }

    // Free the result set
    mysql_free_result(res);
}

void sendMessage(int senderID, int receiverID, const std::string& messageText) {
    std::string query = "INSERT INTO messages (SenderID, ReceiverID, MessageText) VALUES (" +
        std::to_string(senderID) + ", " + std::to_string(receiverID) + ", '" + messageText + "')";

    // Execute the query
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Error: " << mysql_error(conn) << std::endl;
    }
    else {
        std::cout << "Message sent successfully!\n";
    }
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

    // Display address details
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

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void updateProfile(const string& userID) {
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

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

int main() {
    connectToDatabase(); // Establish database connection
    mainMenu(); // Start the main menu
    return 0;
}
