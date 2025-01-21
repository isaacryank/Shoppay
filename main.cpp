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
#include <unordered_map>

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
void sellermanageProducts();
//void adminmanageProducts();
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
void applyCoupon(vector<int>& selectedItems, double& selectedTotalAmount);
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
void viewAndUpdateSellerProfile(const string& sellerID);

void manageProfileInterface();
void manageChatsWithCustomersInterface();
void sellerMessagePageInterface();
void approveCustomerOrdersInterface();
void viewSalesReportsInterface();
void viewTransactionHistoryInterface();
void sellerSalesReportInterface();
void browseProductsInterface();
void generateSalesReportInterface();
void viewCustomerDemographicsInterface();
void viewProductPopularityInterface();
void viewProductDetails(int productID);
void addToCart(int productID, int stock, double price, const string& productName, const string& storeName, int quantity);
void updateCartQuantity(int productID, int newQuantity);
void removeFromCart(int productID);
void checkoutSelectedItems(const vector<int>& selectedItems, double selectedTotalAmount);
void selectItemsForCheckout();
void handlePendingOrders();
void completePendingPayment(int orderId);
void cancelOrder(int orderId);
void viewByStore();
void listCustomers();
void Logout();

void myWalletInterface(); 
void sellerWalletInterface();
void messagePageInterface();
void myCartInterface();
void checkoutInterface();
void paymentTransaction(int orderId, double orderTotalAmount);
void generateReceipt(int receiptId, int orderId, const string& orderDate, const string& customerName, const string& customerEmail, const string& customerAddress, const string& customerPhone, const string& couponCode, double originalAmount, double discountAmount, double subtotal, double grandTotal);
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

int generateOrder(const vector<int>& selectedItems, double selectedTotalAmount);
string fixedPrice(double value);
string centerText(const string& text, int width);
string justifyText(const string& leftText, const string& rightText, int width);


// Utility Functions
void clearScreen() {
#ifdef _WIN32
    system("cls"); // Windows
#else
    system("clear"); // Unix/Linux
#endif
}

void displayBanner() {
    cout << "=============================================================\n";
    cout << "                  WELCOME TO SHOPPAY SYSTEM                  \n";
    cout << "            A PLACE WITH PEACE OF MIND TO SHOPPING           \n";
    cout << "=============================================================\n";

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

// Function to center text
string centerText(const string& text, int width) {
    int padding = (width - text.length()) / 2;
    return string(padding, ' ') + text + string(padding, ' ');
}

// Function to justify text to the left and right
string justifyText(const string& leftText, const string& rightText, int width) {
    int space = width - leftText.length() - rightText.length();
    return leftText + string(space, ' ') + rightText;
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
            //adminmanageProducts();
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
        cout << "| [3] My Wallet                                     |\n";
        cout << "| [4] Manage Products                               |\n";
        cout << "| [5] Approve Customer Orders                       |\n";
        cout << "| [6] Sales Reports                                 |\n";
        cout << "| [7] Transaction History                           |\n";
        cout << "| [8] Seller Sales Report                           |\n";
        cout << "| [9] Logout                                        |\n";
        cout << "=====================================================\n";
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            viewAndUpdateSellerProfile(currentUserID);
            break;
        case 2:
            sellerMessagePageInterface();
            break;
        case 3:
            sellerWalletInterface();
            break;
        case 4:
            sellermanageProducts();
            break;
        case 5:
            approveCustomerOrdersInterface();
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
            browseProducts();
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
void sellermanageProducts() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "=============================================================\n";
        cout << "|                        MANAGE PRODUCTS                    |\n";
        cout << "=============================================================\n";
        cout << " [1] Add Product\n";
        cout << " [2] Update Product\n";
        cout << " [3] Delete Product\n";
        cout << " [4] View All Products\n";
        cout << " [5] Search Products\n";
        cout << " [6] Sort Products\n";
        cout << " [7] Browse Products (Monitor Other Sellers)\n";
        cout << " [8] Back to Seller Menu\n";
        cout << "=============================================================\n";
        cout << " Select an option: ";
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
            sellerMenu(); // Return to main menu;
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
    string name, description;
    double price;
    int stock;

    clearScreen();
    displayBanner();
    cout << "=============================================================\n";
    cout << "|                      EXISTING PRODUCTS                    |\n";
    cout << "=============================================================\n";

    string query = "SELECT * FROM product WHERE SellerID = " + glbStr;
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

    cout << "\n=============================================================\n";
    cout << "|                         ADD PRODUCT                        |\n";
    cout << "=============================================================\n";
    cout << left << setw(20) << "Field" << setw(2) << ": " << "Input" << endl;
    cout << "-------------------------------------------------------------\n";
    cout << left << setw(20) << "Enter Product Name" << setw(2) << ": ";
    cin.ignore();
    getline(cin, name);
    newProduct.name = name;

    cout << left << setw(20) << "Enter Product Description" << setw(2) << ": ";
    getline(cin, description);
    newProduct.description = description;

    cout << left << setw(20) << "Enter Product Price" << setw(2) << ": $";
    cin >> price;
    newProduct.price = price;

    cout << left << setw(20) << "Enter Stock Quantity" << setw(2) << ": ";
    cin >> stock;
    newProduct.stockQuantity = stock;

    query = "INSERT INTO product (ProductName, Description, Price, StockQuantity, SellerID) VALUES ('" +
        newProduct.name + "', '" + newProduct.description + "', " + to_string(newProduct.price) + ", " +
        to_string(newProduct.stockQuantity) + ", " + glbStr + ")";
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product added successfully!\n";
    }

    cout << "=============================================================\n";
    cout << "\nPress Enter to return to the Manage Products Menu...";
    cin.ignore();
    cin.get();
}

void updateProduct() {
    int productId;
    string newValue;
    int choice;

    clearScreen();
    displayBanner();
    cout << "\n=============================================================\n";
    cout << "|                      EXISTING PRODUCTS                    |\n";
    cout << "=============================================================\n";

    string query = "SELECT * FROM product WHERE SellerID = " + glbStr;
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

    cout << "\n=============================================================\n";
    cout << "|                       UPDATE PRODUCT                       |\n";
    cout << "=============================================================\n";
    cout << "[1] Proceed to Update Product\n";
    cout << "[2] Return Back\n";
    cout << "=============================================================\n";
    cout << "YOUR OPTION: ";

    int choice1;
    cin >> choice1;
    cin.ignore(); // Clear the newline character from the input buffer

    if (choice1 == 1) {
        cout << "Enter Product ID to update: ";
        cin >> productId;
        cin.ignore(); // Clear the newline character from the input buffer

        clearScreen();
        displayBanner();
        cout << "=============================================================\n";
        cout << "|                  SELECTED PRODUCT DETAILS                 |\n";
        cout << "=============================================================\n";

        query = "SELECT * FROM product WHERE ProductID = " + to_string(productId) + " AND SellerID = " + glbStr;
        res = executeSelectQuery(query);

        if (res) {
            MYSQL_ROW row;
            if ((row = mysql_fetch_row(res))) {
                cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
                    << setw(10) << "Price" << setw(10) << "Stock" << endl;
                cout << string(100, '=') << endl;
                cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
                    << setw(10) << row[3] << setw(10) << row[4] << endl;

            }
            else {
                cout << "Product not found or you do not have permission to edit this product.\n";
                return;
            }
            mysql_free_result(res);
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
            return;
        }

        while (true) {
            cout << "\n=============================================================\n";
            cout << "|                       UPDATE PRODUCT                       |\n";
            cout << "=============================================================\n";
            cout << "[1] Update Product Name\n";
            cout << "[2] Update Product Description\n";
            cout << "[3] Update Product Price\n";
            cout << "[4] Update Stock Quantity\n";
            cout << "[5] Return to Manage Products Menu\n";
            cout << "=============================================================\n";
            cout << "YOUR OPTION: ";
            cin >> choice;
            cin.ignore(); // Clear the newline character from the input buffer

            switch (choice) {
            case 1:
                cout << "Enter new Product Name: ";
                getline(cin, newValue);
                query = "UPDATE product SET ProductName = '" + newValue + "' WHERE ProductID = " + to_string(productId) + " AND SellerID = " + glbStr;
                break;
            case 2:
                cout << "Enter new Product Description: ";
                getline(cin, newValue);
                query = "UPDATE product SET Description = '" + newValue + "' WHERE ProductID = " + to_string(productId) + " AND SellerID = " + glbStr;
                break;
            case 3:
                cout << "Enter new Product Price: $";
                getline(cin, newValue);
                query = "UPDATE product SET Price = " + newValue + " WHERE ProductID = " + to_string(productId) + " AND SellerID = " + glbStr;
                break;
            case 4:
                cout << "Enter new Stock Quantity: ";
                getline(cin, newValue);
                query = "UPDATE product SET StockQuantity = " + newValue + " WHERE ProductID = " + to_string(productId) + " AND SellerID = " + glbStr;
                break;
            case 5:
                return; // Return to manage products menu
            default:
                cout << "Invalid option. Please try again.\n";
                continue;
            }

            if (executeUpdate(query)) {
                cout << "Product updated successfully!\n";
            }
            else {
                cout << "Error updating product: " << mysql_error(conn) << endl;
            }

            cout << "\nPress Enter to return to the Manage Products Menu...";
            cin.ignore();
            cin.get();
            return;
        }
    }
    else if (choice1 == 2) {
        return; // Return back to the previous menu
    }
    else {
        cout << "Invalid option. Please try again.\n";
    }
}

void deleteProduct() {
    int productId;

    clearScreen();
    displayBanner();
    cout << "=============================================================\n";
    cout << "|                      EXISTING PRODUCTS                    |\n";
    cout << "=============================================================\n";

    string query = "SELECT * FROM product WHERE SellerID = " + glbStr;
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

    cout << "=============================================================\n";
    cout << "|                       DELETE PRODUCT                       |\n";
    cout << "=============================================================\n";
    cout << left << setw(20) << "Enter Product ID to delete" << setw(2) << ": ";
    cin >> productId;

    query = "DELETE FROM product WHERE ProductID = " + to_string(productId) + " AND SellerID = " + glbStr;
    if (!executeUpdate(query)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product deleted successfully!\n";
    }

    cout << "=============================================================\n";
    cout << "\nPress Enter to return to the Manage Products Menu...";
    cin.ignore();
    cin.get();
}

void viewAllProducts() {
    clearScreen();
    displayBanner();
    cout << "=============================================================\n";
    cout << "|                      ALL PRODUCTS                          |\n";
    cout << "=============================================================\n";

    string query = "SELECT * FROM product WHERE SellerID = " + glbStr;
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

    cout << string(100, '=') << endl;
    cout << "\nPress Enter to return to the Manage Products Menu...";
    cin.ignore();
    cin.get();
}

void searchProducts() {
    clearScreen();
    displayBanner();
    cout << "=============================================================\n";
    cout << "|                     SEARCH PRODUCTS                        |\n";
    cout << "=============================================================\n";

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
        "WHERE p.SellerID = " + glbStr + " AND p.ProductName LIKE '%" + keyword + "%' ";

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

    cout << "=============================================================\n";
    cout << "\nPress Enter to return to the Manage Products Menu...";
    cin.ignore();
    cin.get();
}

void sortProducts() {
    int choice;
    string query;

    clearScreen();
    displayBanner();
    cout << "=============================================================\n";
    cout << "|                       SORT PRODUCTS                        |\n";
    cout << "=============================================================\n";
    cout << "[1] Sort by Price\n";
    cout << "[2] Sort by Name\n";
    cout << "=============================================================\n";
    cout << "Select an option: ";
    cin >> choice;

    if (choice == 1) {
        query = "SELECT * FROM product WHERE SellerID = " + glbStr + " ORDER BY Price";
    }
    else if (choice == 2) {
        query = "SELECT * FROM product WHERE SellerID = " + glbStr + " ORDER BY ProductName";
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

    cout << "=============================================================\n";
    cout << "\nPress Enter to return to the Manage Products Menu...";
    cin.ignore();
    cin.get();
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
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== BROWSE PRODUCTS ====================\n";
        string query = "SELECT p.ProductID, p.ProductName, p.Price, p.StockQuantity, u.StoreName FROM product p "
            "INNER JOIN user u ON p.SellerID = u.UserID";
        MYSQL_RES* res = executeSelectQuery(query);

        if (res) {
            MYSQL_ROW row;
            cout << left << setw(10) << "ProductID" << setw(20) << "Store Name" << setw(30) << "Product Name" << setw(10) << "Price" << setw(10) << "Stock" << endl;
            cout << string(80, '-') << endl;

            while ((row = mysql_fetch_row(res))) {
                cout << left << setw(10) << row[0] << setw(20) << row[4] << setw(30) << row[1] << setw(10) << fixed << setprecision(2) << stod(row[2]) << setw(10) << row[3] << endl;
            }
            mysql_free_result(res);
        }
        else {
            cout << "Error: " << mysql_error(conn) << endl;
        }

        cout << "\n[1] Select Product by ID\n";
        cout << "[2] View By Store\n";
        cout << "[3] Return Back\n";
        cout << "Select an option: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            int productID;
            cout << "Enter Product ID to view details: ";
            cin >> productID;
            viewProductDetails(productID);
            break;
        }
        case 2: {
            viewByStore();
            break;
        }
        case 3:
            return; // Return to previous menu
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

void viewByStore() {
    clearScreen();
    displayBanner();

    // Retrieve distinct store names
    string storeQuery = "SELECT DISTINCT u.UserID, u.StoreName FROM product p "
        "INNER JOIN user u ON p.SellerID = u.UserID";
    MYSQL_RES* storeRes = executeSelectQuery(storeQuery);

    if (storeRes) {
        MYSQL_ROW storeRow;
        while ((storeRow = mysql_fetch_row(storeRes))) {
            int storeID = stoi(storeRow[0]);
            string storeName = storeRow[1];

            cout << endl << string(17, '=') << endl;
            cout << "| " << setw(14) << storeName << " |\n";
            cout << string(17, '=') << endl;

            string productQuery = "SELECT ProductID, ProductName, Price, StockQuantity FROM product WHERE SellerID = " + to_string(storeID);
            MYSQL_RES* productRes = executeSelectQuery(productQuery);

            if (productRes) {
                MYSQL_ROW productRow;
                cout << left << setw(10) << "ProductID" << setw(30) << "Product Name" << setw(10) << "Price" << setw(10) << "Stock" << endl;
                cout << string(60, '-') << endl;

                while ((productRow = mysql_fetch_row(productRes))) {
                    cout << left << setw(10) << productRow[0] << setw(30) << productRow[1] << setw(10) << fixed << setprecision(2) << stod(productRow[2]) << setw(10) << productRow[3] << endl;
                }
                mysql_free_result(productRes);
                cout << string(60, '-') << endl;
            }
            else {
                cout << "Error: " << mysql_error(conn) << endl;
            }
        }
        mysql_free_result(storeRes);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "\n[1] Select Product by ID\n";
    cout << "[2] Return Back\n";
    cout << "Select an option: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        int productID;
        cout << "Enter Product ID to view details: ";
        cin >> productID;
        viewProductDetails(productID);
    }
}

void viewProductDetails(int productID) {
    clearScreen();
    displayBanner();
    cout << "==================== PRODUCT DETAILS ====================\n";

    string query = "SELECT p.ProductName, p.Description, p.Price, p.StockQuantity, u.StoreName FROM product p "
        "INNER JOIN user u ON p.SellerID = u.UserID WHERE p.ProductID = " + to_string(productID);
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "Product ID: " << productID << endl;
            cout << "Store Name: " << row[4] << endl;
            cout << "Product Name: " << row[0] << endl;
            cout << "Description: " << row[1] << endl;
            cout << "Price: $" << fixed << setprecision(2) << stod(row[2]) << endl;
            cout << "Stock Quantity: " << row[3] << endl;

            // Prompt to add product to cart
            cout << "\nWould you like to add this product to your cart? (yes/no): ";
            string choice;
            cin >> choice;
            if (choice == "yes") {
                int quantity;
                cout << "Enter quantity: ";
                cin >> quantity;
                addToCart(productID, stoi(row[3]), stod(row[2]), row[0], row[4], quantity);
            }
        }
        else {
            cout << "Product not found." << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }
}

void addToCart(int productID, int stock, double price, const string& productName, const string& storeName, int quantity) {
    // Check if there is enough stock
    if (stock < quantity) {
        cout << "Not enough stock available. Please enter a lower quantity." << endl;
        return;
    }

    // Update stock in the product table
    string updateStockQuery = "UPDATE product SET StockQuantity = StockQuantity - " + to_string(quantity) + " WHERE ProductID = " + to_string(productID);
    if (!executeUpdate(updateStockQuery)) {
        cout << "Failed to update product stock." << endl;
        return;
    }

    // Check if the product is already in the cart
    string checkCartQuery = "SELECT Quantity FROM cart WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
    MYSQL_RES* res = executeSelectQuery(checkCartQuery);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        // Product already in the cart, update quantity
        int existingQuantity = stoi(row[0]);
        int newQuantity = existingQuantity + quantity;
        string updateCartQuery = "UPDATE cart SET Quantity = " + to_string(newQuantity) + ", TotalPrice = " + to_string(newQuantity * price) + " WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
        executeUpdate(updateCartQuery);
        cout << "Product quantity updated in cart successfully!" << endl;
    }
    else {
        // Product is not in the cart, insert new item
        string insertCartQuery = "INSERT INTO cart (UserID, ProductID, Quantity, TotalPrice) VALUES (" + currentUserID + ", " + to_string(productID) + ", " + to_string(quantity) + ", " + to_string(quantity * price) + ")";
        executeUpdate(insertCartQuery);
        cout << "Product added to cart successfully!" << endl;
    }

    mysql_free_result(res);
}

void updateCartQuantity(int productID, int newQuantity) {
    // Fetch the current quantity and price of the product in the cart
    string query = "SELECT Quantity, TotalPrice / Quantity AS UnitPrice FROM cart WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
    MYSQL_RES* res = executeSelectQuery(query);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        int currentQuantity = stoi(row[0]);
        double price = stod(row[1]); // Fetch the unit price

        // Update stock in the product table
        int quantityDifference = newQuantity - currentQuantity;
        string updateStockQuery = "UPDATE product SET StockQuantity = StockQuantity - " + to_string(quantityDifference) + " WHERE ProductID = " + to_string(productID);
        if (!executeUpdate(updateStockQuery)) {
            cout << "Failed to update product stock." << endl;
            mysql_free_result(res);
            return;
        }

        // Update the cart with the new quantity and total price
        string updateCartQuery = "UPDATE cart SET Quantity = " + to_string(newQuantity) + ", TotalPrice = " + to_string(newQuantity * price) + " WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
        if (!executeUpdate(updateCartQuery)) {
            cout << "Failed to update cart." << endl;
        }
        else {
            cout << "Cart quantity updated successfully!" << endl;
        }
    }
    else {
        cout << "Product not found in cart." << endl;
    }

    mysql_free_result(res);
}

void removeFromCart(int productID) {
    // Fetch the current quantity of the product in the cart
    string query = "SELECT Quantity FROM cart WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
    MYSQL_RES* res = executeSelectQuery(query);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        int currentQuantity = stoi(row[0]);

        // Update stock in the product table
        string updateStockQuery = "UPDATE product SET StockQuantity = StockQuantity + " + to_string(currentQuantity) + " WHERE ProductID = " + to_string(productID);
        if (!executeUpdate(updateStockQuery)) {
            cout << "Failed to update product stock." << endl;
            return;
        }

        // Remove the product from the cart
        string removeCartQuery = "DELETE FROM cart WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
        executeUpdate(removeCartQuery);
        cout << "Product removed from cart successfully!" << endl;
    }
    else {
        cout << "Product not found in cart." << endl;
    }

    mysql_free_result(res);
}

void viewCartWithChatOption() {
    clearScreen();
    displayBanner();
    cout << "==================== VIEW CART ====================\n";

    // Query to fetch cart items for the current user
    string query = "SELECT c.ProductID, p.ProductName, u.StoreName, c.Quantity, c.TotalPrice "
        "FROM cart c INNER JOIN product p ON c.ProductID = p.ProductID "
        "INNER JOIN user u ON p.SellerID = u.UserID "
        "WHERE c.UserID = " + currentUserID;

    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        if (mysql_num_rows(res) == 0) {
            cout << "Your cart is empty." << endl;
        }
        else {
            // Print table header
            cout << left << setw(10) << "Product ID" << setw(30) << "Product Name" << setw(20) << "Store Name" << setw(10) << "Quantity" << setw(10) << "Price" << setw(10) << "Total" << endl;
            cout << string(90, '-') << endl;

            while ((row = mysql_fetch_row(res))) {
                int productID = stoi(row[0]);
                string productName = row[1];
                string storeName = row[2];
                int quantity = stoi(row[3]);
                double price = stod(row[4]);
                double total = price * quantity;

                // Print each cart item
                cout << left << setw(10) << productID << setw(30) << productName << setw(20) << storeName << setw(10) << quantity << setw(10) << fixed << setprecision(2) << price << setw(10) << fixed << setprecision(2) << total << endl;
            }
            mysql_free_result(res);
        }
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    int productID, sellerID;
    string message;

    cout << "Enter Product ID to chat about: ";
    cin >> productID;

    // Get the SellerID for the selected product
    query = "SELECT p.SellerID FROM product p WHERE p.ProductID = " + to_string(productID);
    res = executeSelectQuery(query);
    MYSQL_ROW row; // Declaration of row variable
    if (res && (row = mysql_fetch_row(res))) {
        sellerID = stoi(row[0]);
        mysql_free_result(res);
    }
    else {
        cout << "Invalid Product ID. Please try again.\n";
        return;
    }

    cin.ignore(); // Clear the newline character from the input buffer
    cout << "Enter your message: ";
    getline(cin, message);

    sendMessage(stoi(currentUserID), sellerID, message); // Send message from current user to seller
    viewMessages(stoi(currentUserID), sellerID); // View updated chat history with the seller

    cout << "\nPress Enter to return to the My Cart Menu...";
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
    displayBanner();

    // Query to get the user's profile information
    string query = "SELECT UserID, Username, Email, FullName, Address, PhoneNumber, Password FROM user WHERE UserID = " + userID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            // Display user's profile information
            cout << "=============================================================\n";
            cout << "|                         PROFILE DETAILS                    |\n";
            cout << "=============================================================\n";
            cout << "Username: " << row[1] << "\n";
            cout << "Email: " << row[2] << "\n";
            cout << "Full Name: " << row[3] << "\n";
            cout << "Address: " << row[4] << "\n";
            cout << "Phone Number: " << row[5] << "\n";
            cout << "Password: " << "********" << "\n"; // Password is masked for security
            cout << "=============================================================\n";

            // Provide options to update profile
            cout << "[1] Update Username\n";
            cout << "[2] Update Email\n";
            cout << "[3] Update Full Name\n";
            cout << "[4] Update Address\n";
            cout << "[5] Update Phone Number\n";
            cout << "[6] Update Password\n";
            cout << "[7] Return to Main Menu\n";
            cout << "\nYOUR OPTION: ";

            int option;
            cin >> option;
            cin.ignore(); // Clear the newline character from the input buffer

            string newValue;
            switch (option) {
            case 1:
                cout << "Enter new Username: ";
                getline(cin, newValue);
                query = "UPDATE user SET Username = '" + newValue + "' WHERE UserID = " + userID;
                break;
            case 2:
                cout << "Enter new Email: ";
                getline(cin, newValue);
                query = "UPDATE user SET Email = '" + newValue + "' WHERE UserID = " + userID;
                break;
            case 3:
                cout << "Enter new Full Name: ";
                getline(cin, newValue);
                query = "UPDATE user SET FullName = '" + newValue + "' WHERE UserID = " + userID;
                break;
            case 4:
                cout << "Enter new Address: ";
                getline(cin, newValue);
                query = "UPDATE user SET Address = '" + newValue + "' WHERE UserID = " + userID;
                break;
            case 5:
                cout << "Enter new Phone Number: ";
                getline(cin, newValue);
                query = "UPDATE user SET PhoneNumber = '" + newValue + "' WHERE UserID = " + userID;
                break;
            case 6:
                cout << "Enter new Password: ";
                getline(cin, newValue);
                newValue = bcrypt::generateHash(newValue); // Hash the password before storing
                query = "UPDATE user SET Password = '" + newValue + "' WHERE UserID = " + userID;
                break;
            case 7:
                return; // Return to main menu
            default:
                cout << "Invalid option. Returning to main menu.\n";
                return;
            }

            // Execute the update query
            if (executeUpdate(query)) {
                cout << "Profile updated successfully!\n";
            }
            else {
                cout << "Error updating profile: " << mysql_error(conn) << endl;
            }

            // Display updated profile
            viewAndUpdateProfile(userID);
        }
        else {
            cout << "User not found.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "\nPress Enter to return to the Main Menu...";
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
        cout << "====================================================\n";
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

void sellerWalletInterface() {
    int choice;
    while (true) {
        clearScreen();
        displayBanner();
        cout << "==================== MY WALLET ====================\n";
        cout << "| [1] Create E-Wallet Account                      |\n";
        cout << "| [2] Top-Up Wallet                                |\n";
        cout << "| [3] View Wallet Balance                          |\n";
        cout << "| [4] Back to Seller Menu                          |\n";
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
            return; // Back to Seller Menu
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        cin.ignore();
        cin.get();
    }
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

void myCartInterface() {
    clearScreen();
    displayBanner();
    cout << "==================== VIEW CART ====================\n";

    // Query to fetch cart items for the current user
    string query = "SELECT c.ProductID, p.ProductName, u.StoreName, c.Quantity, c.TotalPrice "
        "FROM cart c INNER JOIN product p ON c.ProductID = p.ProductID "
        "INNER JOIN user u ON p.SellerID = u.UserID "
        "WHERE c.UserID = " + currentUserID;

    MYSQL_RES* res = executeSelectQuery(query);

    vector<int> selectedItems; // Initialize selectedItems
    double selectedTotalAmount = 0.0; // Initialize selectedTotalAmount

    if (res) {
        MYSQL_ROW row;
        if (mysql_num_rows(res) == 0) {
            cout << "Your cart is empty." << endl;
        }
        else {
            // Print table header
            cout << left << setw(10) << "Product ID" << setw(30) << "Product Name" << setw(20) << "Store Name" << setw(10) << "Quantity" << setw(10) << "Price" << setw(10) << "Total" << endl;
            cout << string(90, '-') << endl;

            while ((row = mysql_fetch_row(res))) {
                int productID = stoi(row[0]);
                string productName = row[1];
                string storeName = row[2];
                int quantity = stoi(row[3]);
                double price = stod(row[4]);
                double total = price * quantity;

                // Print each cart item
                cout << left << setw(10) << productID << setw(30) << productName << setw(20) << storeName << setw(10) << quantity << setw(10) << fixed << setprecision(2) << price << setw(10) << fixed << setprecision(2) << total << endl;

                selectedItems.push_back(productID); // Add productID to selectedItems
                selectedTotalAmount += total; // Add total to selectedTotalAmount
            }
            mysql_free_result(res);

            // Print total amount
            cout << string(90, '=') << endl;
            cout << right << setw(70) << "Total Amount: $" << fixed << setprecision(2) << selectedTotalAmount << endl;
        }
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "\n==================== MY CART ====================\n";
    cout << "| [1] Update Quantity                           |\n";
    cout << "| [2] Remove Item                               |\n";
    cout << "| [3] Apply Coupon                              |\n";
    cout << "| [4] Chat Seller                               |\n";
    cout << "| [5] Select Items for Checkout                 |\n";
    cout << "| [6] Back to Customer Menu                     |\n";
    cout << "=================================================\n";
    cout << "Select an option: ";

    int option;
    cin >> option;

    switch (option) {
    case 1: {
        int productID, newQuantity;
        cout << "Enter Product ID to update quantity: ";
        cin >> productID;
        cout << "Enter new quantity: ";
        cin >> newQuantity;
        updateCartQuantity(productID, newQuantity);
        break;
    }
    case 2: {
        int productID;
        cout << "Enter Product ID to remove: ";
        cin >> productID;
        removeFromCart(productID);
        break;
    }
    case 3: {
        string couponCode;
        cout << "Enter Coupon Code: ";
        cin >> couponCode;
        applyCoupon(selectedItems, selectedTotalAmount); // Pass selectedItems and selectedTotalAmount to applyCoupon
        break;
    }
    case 4: {
        viewCartWithChatOption(); // Assuming viewCartWithChatOption does not take any arguments
        break;
    }
    case 5:
        selectItemsForCheckout(); // handle item selection for checkout
        break;
    case 6:
        // Back to Customer Menu
        return;
    default:
        cout << "Invalid option. Please try again." << endl;
        break;
    }

    cout << "\nPress Enter to return to the My Cart Menu...";
    cin.ignore();
    cin.get();
}


void viewCart() {
    clearScreen();
    displayBanner();
    cout << "==================== VIEW CART ====================\n";

    // Query to fetch cart items for the current user
    string query = "SELECT c.ProductID, p.ProductName, u.StoreName, c.Quantity, c.TotalPrice "
        "FROM cart c INNER JOIN product p ON c.ProductID = p.ProductID "
        "INNER JOIN user u ON p.SellerID = u.UserID "
        "WHERE c.UserID = " + currentUserID;

    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        if (mysql_num_rows(res) == 0) {
            cout << "Your cart is empty." << endl;
        }
        else {
            // Print table header
            cout << left << setw(10) << "Product ID" << setw(30) << "Product Name" << setw(20) << "Store Name" << setw(10) << "Quantity" << setw(10) << "Price" << setw(10) << "Total" << endl;
            cout << string(90, '-') << endl;

            double totalAmount = 0.0;
            while ((row = mysql_fetch_row(res))) {
                int productID = stoi(row[0]);
                string productName = row[1];
                string storeName = row[2];
                int quantity = stoi(row[3]);
                double price = stod(row[4]);
                double total = price * quantity;

                // Print each cart item
                cout << left << setw(10) << productID << setw(30) << productName << setw(20) << storeName << setw(10) << quantity << setw(10) << fixed << setprecision(2) << price << setw(10) << fixed << setprecision(2) << total << endl;

                totalAmount += total;
            }
            mysql_free_result(res);

            // Print total amount
            cout << string(90, '=') << endl;
            cout << right << setw(70) << "Total Amount: $" << fixed << setprecision(2) << totalAmount << endl;
        }
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "\nPress Enter to return to the My Cart Menu...";
    cin.ignore();
    cin.get();
}


void applyCoupon(vector<int>& selectedItems, double& selectedTotalAmount) {
    clearScreen();
    displayBanner();
    cout << "==================== APPLY COUPON ====================\n";
    string couponCode;
    cout << "Enter coupon code: ";
    cin >> couponCode;

    // Query to fetch coupon details from the database
    string query = "SELECT DiscountPercentage FROM coupon WHERE Code = '" + couponCode + "' AND ValidFrom <= CURDATE() AND ValidUntil >= CURDATE() AND (UsageLimit IS NULL OR UsageLimit > 0)";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            double discountPercentage = stod(row[0]);
            double discount = selectedTotalAmount * (discountPercentage / 100);
            selectedTotalAmount -= discount;

            // Update coupon usage limit if applicable
            string updateQuery = "UPDATE coupon SET UsageLimit = UsageLimit - 1 WHERE Code = '" + couponCode + "' AND UsageLimit IS NOT NULL";
            executeUpdate(updateQuery);

            cout << "\n=====================================================\n";
            cout << "Coupon applied successfully!" << endl;
            cout << "You saved: $" << fixed << setprecision(2) << discount << endl;
            cout << "New total amount: $" << fixed << setprecision(2) << selectedTotalAmount << endl;
            cout << "=====================================================\n";
        }
        else {
            cout << "\n=====================================================\n";
            cout << "Invalid or expired coupon code." << endl;
            cout << "=====================================================\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "\n=====================================================\n";
        cout << "Error: " << mysql_error(conn) << endl;
        cout << "=====================================================\n";
    }

    cout << "\nPress Enter to return to the Checkout Menu...";
    cin.ignore();
    cin.get();

    // Proceed to checkout after applying coupon
    checkoutSelectedItems(selectedItems, selectedTotalAmount);
}

void checkoutInterface() {
    clearScreen();
    displayBanner();
    cout << "==================== CHECKOUT ====================\n";

    // Fetch items from the cart table for the current user
    string query = "SELECT c.ProductID, c.Quantity, c.TotalPrice / c.Quantity AS UnitPrice, p.ProductName, u.StoreName "
        "FROM cart c "
        "INNER JOIN product p ON c.ProductID = p.ProductID "
        "INNER JOIN user u ON p.SellerID = u.UserID "
        "WHERE c.UserID = " + currentUserID;
    MYSQL_RES* res = executeSelectQuery(query);
    MYSQL_ROW row;

    if (res && mysql_num_rows(res) > 0) {
        cout << "Proceeding to checkout..." << endl;

        // Calculate the total amount from the cart items
        double totalAmount = 0.0;
        vector<int> productIDs;
        while ((row = mysql_fetch_row(res))) {
            int quantity = stoi(row[1]);
            double unitPrice = stod(row[2]);
            totalAmount += quantity * unitPrice;
            productIDs.push_back(stoi(row[0]));
        }

        // Use a valid StatusID (e.g., 1 for 'Pending Seller Approval')
        int validStatusID = 1;

        // Generate a new order and save to the database
        string insertOrderQuery = "INSERT INTO orders (UserID, TotalAmount, StatusID, OrderDate) VALUES (" + currentUserID + ", " + to_string(totalAmount) + ", " + to_string(validStatusID) + ", NOW())";

        if (executeUpdate(insertOrderQuery)) {
            // Retrieve the last inserted order ID
            query = "SELECT LAST_INSERT_ID()";
            MYSQL_RES* resLastInsertId = executeSelectQuery(query);
            MYSQL_ROW rowLastInsertId = mysql_fetch_row(resLastInsertId);
            int orderId = stoi(rowLastInsertId[0]);
            mysql_free_result(resLastInsertId);

            // Insert items into the order_items table
            query = "SELECT c.ProductID, c.Quantity, c.TotalPrice / c.Quantity AS UnitPrice, p.ProductName, u.StoreName "
                "FROM cart c "
                "INNER JOIN product p ON c.ProductID = p.ProductID "
                "INNER JOIN user u ON p.SellerID = u.UserID "
                "WHERE c.UserID = " + currentUserID;
            res = executeSelectQuery(query);
            while ((row = mysql_fetch_row(res))) {
                int productID = stoi(row[0]);
                int quantity = stoi(row[1]);
                double unitPrice = stod(row[2]);
                string productName = row[3];
                string storeName = row[4];

                string itemQuery = "INSERT INTO order_items (OrderID, ProductID, Quantity, UnitPrice, ProductName, StoreName) "
                    "VALUES (" + to_string(orderId) + ", " + to_string(productID) + ", " + to_string(quantity) + ", " + to_string(unitPrice) + ", '" + productName + "', '" + storeName + "')";
                executeUpdate(itemQuery);
            }

            // Clear the cart table
            string clearCartQuery = "DELETE FROM cart WHERE UserID = " + currentUserID;
            executeUpdate(clearCartQuery);

            cout << "Order placed successfully! Your order ID is " << orderId << "." << endl;
            cout << "Proceed to Payment Transaction" << endl;
            paymentTransaction(orderId, totalAmount); // Call with two arguments
        }
        else {
            cout << "Error placing order: " << mysql_error(conn) << endl;
        }
    }
    else {
        cout << "Your cart is empty. Please add products to the cart before checking out." << endl;
    }

    mysql_free_result(res);
    cout << "\nPress Enter to return to the Customer Menu...";
    cin.ignore();
    cin.get();
}

void paymentTransaction(int orderId) {
    clearScreen();
    displayBanner();
    cout << "==================== PAYMENT TRANSACTION ====================\n";

    // Fetch the total amount for the order
    string query = "SELECT TotalAmount FROM orders WHERE OrderID = " + to_string(orderId);
    MYSQL_RES* res = executeSelectQuery(query);
    MYSQL_ROW row = mysql_fetch_row(res);
    double totalAmount = 0.0;
    if (row) {
        totalAmount = stod(row[0]);
    }
    mysql_free_result(res);

    query = "SELECT Balance FROM wallet WHERE UserID = " + currentUserID;
    res = executeSelectQuery(query);

    if (res) {
        row = mysql_fetch_row(res);
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

                    string updateOrder = "UPDATE orders SET OrderStatus = 'Completed' WHERE OrderID = " + to_string(orderId);
                    executeUpdate(updateOrder);

                    cout << "Payment successful! Order completed." << endl;

                    // Fetch additional details for the receipt
                    query = "SELECT UserID, OrderDate FROM orders WHERE OrderID = " + to_string(orderId);
                    res = executeSelectQuery(query);
                    row = mysql_fetch_row(res);
                    string orderDate = row[1];
                    string customerName = ""; // Fetch customer name from user table
                    string customerEmail = ""; // Fetch customer email from user table
                    string customerAddress = ""; // Fetch customer address from user table
                    string customerPhone = ""; // Fetch customer phone from user table

                    mysql_free_result(res);

                    // Fetch customer details
                    query = "SELECT FullName, Email, Address, PhoneNumber FROM user WHERE UserID = " + currentUserID;
                    res = executeSelectQuery(query);
                    if (res) {
                        row = mysql_fetch_row(res);
                        customerName = row[0];
                        customerEmail = row[1];
                        customerAddress = row[2];
                        customerPhone = row[3];
                        mysql_free_result(res);
                    }

                    // Fetch coupon details (if any)
                    string couponCode = "";
                    double discountAmount = 0.0;
                    query = "SELECT CouponCode, DiscountAmount FROM orders WHERE OrderID = " + to_string(orderId);
                    res = executeSelectQuery(query);
                    if (res) {
                        row = mysql_fetch_row(res);
                        couponCode = row[0];
                        discountAmount = stod(row[1]);
                        mysql_free_result(res);
                    }

                    double subtotal = totalAmount - discountAmount;
                    double grandTotal = subtotal; // Assuming no additional charges

                    // Generate the receipt
                    generateReceipt(orderId, orderId, orderDate, customerName, customerEmail, customerAddress, customerPhone, couponCode, totalAmount, discountAmount, subtotal, grandTotal);
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

void generateReceipt(int receiptId, int orderId, const string& orderDate, const string& customerName, const string& customerEmail, const string& customerAddress, const string& customerPhone, const string& couponCode, double originalAmount, double discountAmount, double subtotal, double grandTotal) {
    const int width = 65; // Width of console output
    stringstream receipt; // Use stringstream to collect the receipt content

    receipt << "=============================================================\n";
    receipt << centerText("SHOPPAY SYSTEM", width) << "\n";
    receipt << centerText("A PLACE WITH PEACE OF MIND TO SHOPPING", width) << "\n";
    receipt << "=============================================================\n";
    receipt << centerText("RECEIPT", width) << "\n\n";

    receipt << "Receipt number : receipt_" << receiptId << "\n";
    receipt << "Order Number : Order_" << orderId << "\n";
    receipt << "Date of purchase: " << orderDate << "\n\n";

    receipt << centerText("Customer Signature", width) << "\n";
    receipt << justifyText("Fullname: " + customerName, "Email: " + customerEmail, width) << "\n";
    receipt << justifyText("Address: " + customerAddress, "No Phone: " + customerPhone, width) << "\n\n";

    // Sample store data, you can replace this with actual data fetching logic
    receipt << "Store: GOJODOQ\n";
    receipt << "Email: store1@example.com\n";
    receipt << "-----------------------------------------------------------------\n";
    receipt << left << setw(30) << "Product Name" << setw(10) << "Quantity" << setw(10) << "Price" << setw(15) << "Total" << endl;
    receipt << "-----------------------------------------------------------------\n";
    receipt << left << setw(30) << "MINI POWERBANK" << setw(10) << "2" << setw(10) << "89.00" << setw(15) << "178.00" << endl;
    receipt << "-----------------------------------------------------------------\n";

    receipt << "Store: UGREEN\n";
    receipt << "Email: store2@example.com\n";
    receipt << "-----------------------------------------------------------------\n";
    receipt << left << setw(30) << "Product Name" << setw(10) << "Quantity" << setw(10) << "Price" << setw(15) << "Total" << endl;
    receipt << "-----------------------------------------------------------------\n";
    receipt << left << setw(30) << "EARBUDS" << setw(10) << "1" << setw(10) << "25.00" << setw(15) << "25.00" << endl;
    receipt << "-----------------------------------------------------------------\n";

    receipt << "Store: DIY\n";
    receipt << "Email: store3@example.com\n";
    receipt << "-----------------------------------------------------------------\n";
    receipt << left << setw(30) << "Product Name" << setw(10) << "Quantity" << setw(10) << "Price" << setw(15) << "Total" << endl;
    receipt << "-----------------------------------------------------------------\n";
    receipt << left << setw(30) << "CLAY POT" << setw(10) << "3" << setw(10) << "3.00" << setw(15) << "9.00" << endl;
    receipt << "-----------------------------------------------------------------\n";

    receipt << "-----------------------------------------------------------------\n";
    receipt << justifyText("Amount:", "$" + to_string(originalAmount), width) << "\n";
    receipt << justifyText("Coupon Applied:", couponCode, width) << "\n";
    receipt << justifyText("Coupon:", "- $" + to_string(discountAmount), width) << "\n";
    receipt << justifyText(" ", "---------------", width) << "\n";
    receipt << justifyText("Subtotal:", "$" + to_string(subtotal), width) << "\n";
    receipt << justifyText(" ", "---------------", width) << "\n";
    receipt << justifyText("Grand Total:", "$" + to_string(grandTotal), width) << "\n";
    receipt << "===============================================================\n";

    // Print the receipt to the console
    cout << receipt.str();

    // Save the receipt to a text file
    ofstream receiptFile("receipt_" + to_string(receiptId) + ".txt");
    if (receiptFile.is_open()) {
        receiptFile << receipt.str();
        receiptFile.close();
        cout << "Receipt saved to receipt_" << receiptId << ".txt" << endl;
    }
    else {
        cout << "Error opening file to save receipt." << endl;
    }
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
    cout << "| [1] Handle Pending Orders                             |\n";
    cout << "| [2] Back to Customer Menu                             |\n";
    cout << "========================================================\n";
    cout << "Select an option: ";

    int option;
    cin >> option;

    switch (option) {
    case 1:
        handlePendingOrders(); // New function to handle pending orders
        break;
    case 2:
        // Back to Customer Menu
        return;
    default:
        cout << "Invalid option. Please try again." << endl;
        break;
    }

    cout << "\nPress Enter to return to the My Orders Menu...";
    cin.ignore();
    cin.get();
}

// New function definition for seller profile update
void viewAndUpdateSellerProfile(const string& sellerID) {
    clearScreen();
    displayBanner();

    // Query to get the seller's profile information
    string query = "SELECT UserID, StoreName, Username, Email, PhoneNumber, Password FROM user WHERE UserID = " + sellerID;
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            // Display seller's profile information
            cout << "=============================================================\n";
            cout << "|                      SELLER PROFILE                        |\n";
            cout << "=============================================================\n";
            cout << "Store Name: " << (row[1] ? row[1] : "Unknown Store") << "\n";
            cout << "User ID: " << row[0] << "\n";
            cout << "Username: " << row[2] << "\n";
            cout << "Email: " << row[3] << "\n";
            cout << "Phone Number: " << row[4] << "\n";
            cout << "Password: " << "********" << "\n"; // Password is masked for security
            cout << "=============================================================\n";

            // Provide options to update profile
            cout << "[1] Update Store Name\n";
            cout << "[2] Update Username\n";
            cout << "[3] Update Email\n";
            cout << "[4] Update Phone Number\n";
            cout << "[5] Update Password\n";
            cout << "[6] Return to Main Menu\n";
            cout << "\nYOUR OPTION: ";

            int option;
            cin >> option;
            cin.ignore(); // Clear the newline character from the input buffer

            string newValue;
            switch (option) {
            case 1:
                cout << "Enter new Store Name: ";
                getline(cin, newValue);
                query = "UPDATE user SET StoreName = '" + newValue + "' WHERE UserID = " + sellerID;
                break;
            case 2:
                cout << "Enter new Username: ";
                getline(cin, newValue);
                query = "UPDATE user SET Username = '" + newValue + "' WHERE UserID = " + sellerID;
                break;
            case 3:
                cout << "Enter new Email: ";
                getline(cin, newValue);
                query = "UPDATE user SET Email = '" + newValue + "' WHERE UserID = " + sellerID;
                break;
            case 4:
                cout << "Enter new Phone Number: ";
                getline(cin, newValue);
                query = "UPDATE user SET PhoneNumber = '" + newValue + "' WHERE UserID = " + sellerID;
                break;
            case 5:
                cout << "Enter new Password: ";
                getline(cin, newValue);
                newValue = bcrypt::generateHash(newValue); // Hash the password before storing
                query = "UPDATE user SET Password = '" + newValue + "' WHERE UserID = " + sellerID;
                break;
            case 6:
                return; // Return to main menu
            default:
                cout << "Invalid option. Returning to main menu.\n";
                return;
            }

            // Execute the update query
            if (executeUpdate(query)) {
                cout << "Profile updated successfully!\n";
            }
            else {
                cout << "Error updating profile: " << mysql_error(conn) << endl;
            }

            // Display updated profile
            viewAndUpdateSellerProfile(sellerID);
        }
        else {
            cout << "Seller not found.\n";
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "\nPress Enter to return to the Main Menu...";
    cin.ignore();
    cin.get();
}

void sellerMessagePageInterface() {
    clearScreen();
    displayBanner();

    // Query to get the messages sent to the current seller
    string query = "SELECT u.UserID, u.Username, m.SenderID, m.MessageText, m.Timestamp "
        "FROM messages m "
        "INNER JOIN user u ON m.SenderID = u.UserID "
        "WHERE m.ReceiverID = " + currentUserID + " "
        "ORDER BY m.Timestamp DESC";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        if (mysql_num_rows(res) == 0) {
            // No messages found
            cout << "=============================================================\n";
            cout << "|                         MESSAGES                           |\n";
            cout << "=============================================================\n";
            cout << "\nTHERE'S NO MESSAGES FROM CUSTOMERS.\n";
            cout << "=============================================================\n";
            cout << "[1] RETURN BACK\n";
            cout << "\nYOUR OPTION: ";

            int option;
            cin >> option;
            if (option == 1) {
                return;
            }
        }
        else {
            // Messages found
            cout << "=============================================================\n";
            cout << "|                         MESSAGES                           |\n";
            cout << "=============================================================\n";

            set<int> displayedCustomers;

            while ((row = mysql_fetch_row(res))) {
                int customerID = stoi(row[0]);
                string username = row[1] ? row[1] : "Unknown User";

                // Only display the latest message from each customer
                if (displayedCustomers.find(customerID) == displayedCustomers.end()) {
                    cout << "- [" << username << "]  [ CustomerID = '" << customerID << "']\n";
                    cout << "Customer: " << row[3] << " [" << row[4] << "]\n\n";
                    displayedCustomers.insert(customerID);
                }
            }

            cout << "=============================================================\n";
            cout << "[1] SELECT YOUR MESSAGE\n";
            cout << "[2] RETURN BACK\n";
            cout << "\nYOUR OPTION: ";

            int option;
            cin >> option;
            if (option == 1) {
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

void selectItemsForCheckout() {
    clearScreen();
    displayBanner();
    cout << "==================== SELECT ITEMS FOR CHECKOUT ====================\n";

    // Query to fetch cart items for the current user
    string query = "SELECT c.ProductID, p.ProductName, u.StoreName, c.Quantity, c.TotalPrice "
        "FROM cart c INNER JOIN product p ON c.ProductID = p.ProductID "
        "INNER JOIN user u ON p.SellerID = u.UserID "
        "WHERE c.UserID = " + currentUserID;

    MYSQL_RES* res = executeSelectQuery(query);
    std::unordered_map<int, std::pair<int, double>> cartItems; // Map to store cart items: ProductID -> (Quantity, TotalPrice)
    double selectedTotalAmount = 0.0;

    if (res && mysql_num_rows(res) > 0) {
        MYSQL_ROW row;
        cout << left << setw(10) << "Product ID" << setw(30) << "Product Name" << setw(20) << "Store Name" << setw(10) << "Quantity" << setw(10) << "Price" << setw(10) << "Total" << endl;
        cout << string(90, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            int productID = stoi(row[0]);
            string productName = row[1];
            string storeName = row[2];
            int quantity = stoi(row[3]);
            double price = stod(row[4]);
            double total = price * quantity;

            // Print each cart item
            cout << left << setw(10) << productID << setw(30) << productName << setw(20) << storeName << setw(10) << quantity << setw(10) << fixed << setprecision(2) << price << setw(10) << fixed << setprecision(2) << total << endl;

            // Store cart item details in map
            cartItems[productID] = { quantity, total };
        }
        mysql_free_result(res);

        cout << string(90, '=') << endl;
        cout << "Enter Product IDs to checkout (comma separated): ";
        string input;
        cin.ignore();
        getline(cin, input);

        stringstream ss(input);
        string item;
        vector<int> selectedItems;
        while (getline(ss, item, ',')) {
            int productID = stoi(item);
            selectedItems.push_back(productID);
        }

        // Calculate total amount for selected items
        for (int productID : selectedItems) {
            if (cartItems.find(productID) != cartItems.end()) {
                selectedTotalAmount += cartItems[productID].second;
            }
        }

        cout << "Total Amount for Selected Items: $" << fixed << setprecision(2) << selectedTotalAmount << endl;
        cout << "[1] Proceed to Checkout\n";
        cout << "[2] Apply Coupon\n"; // New option to apply a coupon
        cout << "[3] Cancel\n";
        cout << "Select an option: ";
        int option;
        cin >> option;

        if (option == 1) {
            checkoutSelectedItems(selectedItems, selectedTotalAmount);
        }
        else if (option == 2) {
            applyCoupon(selectedItems, selectedTotalAmount);
        }
    }
    else {
        cout << "Your cart is empty. Please add products to the cart before selecting items for checkout." << endl;
    }

    cout << "\nPress Enter to return to the My Cart Menu...";
    cin.ignore();
    cin.get();
}

void checkoutSelectedItems(const vector<int>& selectedItems, double selectedTotalAmount) {
    clearScreen();
    displayBanner();
    cout << "==================== CHECKOUT ====================\n";

    // Fetch details of selected items from the cart
    string query = "SELECT c.ProductID, p.ProductName, u.StoreName, c.Quantity, c.TotalPrice / c.Quantity AS UnitPrice "
        "FROM cart c INNER JOIN product p ON c.ProductID = p.ProductID "
        "INNER JOIN user u ON p.SellerID = u.UserID "
        "WHERE c.UserID = " + currentUserID + " AND c.ProductID IN (";
    for (size_t i = 0; i < selectedItems.size(); ++i) {
        query += to_string(selectedItems[i]);
        if (i < selectedItems.size() - 1) {
            query += ", ";
        }
    }
    query += ")";

    MYSQL_RES* res = executeSelectQuery(query);

    if (res && mysql_num_rows(res) > 0) {
        MYSQL_ROW row;
        cout << left << setw(10) << "Product ID" << setw(30) << "Product Name" << setw(20) << "Store Name" << setw(10) << "Quantity" << setw(10) << "Unit Price" << setw(10) << "Total" << endl;
        cout << string(90, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            int productID = stoi(row[0]);
            string productName = row[1];
            string storeName = row[2];
            int quantity = stoi(row[3]);
            double unitPrice = stod(row[4]);
            double total = unitPrice * quantity;

            // Print each selected item
            cout << left << setw(10) << productID << setw(30) << productName << setw(20) << storeName << setw(10) << quantity << setw(10) << fixed << setprecision(2) << unitPrice << setw(10) << fixed << setprecision(2) << total << endl;
        }
        mysql_free_result(res);

        cout << string(90, '=') << endl;
        cout << "Total Amount: $" << fixed << setprecision(2) << selectedTotalAmount << endl;
        cout << "\n[1] Proceed with Payment\n";
        cout << "[2] Cancel\n";
        cout << "Select an option: ";
        int option;
        cin >> option;

        if (option == 1) {
            // Generate a new order and proceed to payment
            int orderId = generateOrder(selectedItems, selectedTotalAmount);
            if (orderId > 0) {
                paymentTransaction(orderId, selectedTotalAmount);
            }
        }
        else {
            cout << "Checkout cancelled." << endl;
        }
    }
    else {
        cout << "Error fetching selected items: " << mysql_error(conn) << endl;
    }

    cout << "\nPress Enter to return to the Customer Menu...";
    cin.ignore();
    cin.get();
}

void paymentTransaction(int orderId, double orderTotalAmount) {
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
            cout << "Order Total: $" << fixed << setprecision(2) << orderTotalAmount << endl;

            if (walletBalance >= orderTotalAmount) {
                cout << "Confirm payment? (yes/no): ";
                string confirm;
                cin >> confirm;
                if (confirm == "yes") {
                    // Deduct amount from wallet and update order status
                    string updateWallet = "UPDATE wallet SET Balance = Balance - " + to_string(orderTotalAmount) + " WHERE UserID = " + currentUserID;
                    executeUpdate(updateWallet);

                    string updateOrder = "UPDATE orders SET StatusID = 2 WHERE OrderID = " + to_string(orderId); // StatusID 2 for 'Completed'
                    executeUpdate(updateOrder);

                    cout << "Payment successful! Order completed." << endl;
                }
                else {
                    cout << "Payment cancelled." << endl;
                }
            }
            else {
                cout << "Insufficient wallet balance. Redirecting to 'My Order' to complete the payment later." << endl;
                string updateOrder = "UPDATE orders SET StatusID = 3 WHERE OrderID = " + to_string(orderId); // StatusID 3 for 'Pending Payment'
                executeUpdate(updateOrder);
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

    cout << "\nPress Enter to return to the Customer Menu...";
    cin.ignore();
    cin.get();
}

void handlePendingOrders() {
    clearScreen();
    displayBanner();
    cout << "==================== PENDING ORDERS ====================\n";

    string query = "SELECT OrderID, TotalAmount, OrderStatus FROM orders WHERE UserID = " + currentUserID + " AND OrderStatus = 'Pending'";
    MYSQL_RES* res = executeSelectQuery(query);

    if (res) {
        MYSQL_ROW row;
        if (mysql_num_rows(res) == 0) {
            cout << "You have no pending orders." << endl;
        }
        else {
            cout << left << setw(10) << "Order ID" << setw(20) << "Total Amount" << setw(20) << "Status" << endl;
            cout << string(50, '-') << endl;

            while ((row = mysql_fetch_row(res))) {
                cout << left << setw(10) << row[0] << setw(20) << fixed << setprecision(2) << stod(row[1]) << setw(20) << row[2] << endl;
            }
            mysql_free_result(res);

            cout << "\n[1] Complete Payment\n";
            cout << "[2] Cancel Order\n";
            cout << "[3] Back to Customer Menu\n";
            cout << "Select an option: ";
            int option;
            cin >> option;

            if (option == 1) {
                int orderId;
                cout << "Enter Order ID to complete payment: ";
                cin >> orderId;
                completePendingPayment(orderId);
            }
            else if (option == 2) {
                int orderId;
                cout << "Enter Order ID to cancel: ";
                cin >> orderId;
                cancelOrder(orderId);
            }
        }
    }
    else {
        cout << "Error: " << mysql_error(conn) << endl;
    }

    cout << "\nPress Enter to return to the Customer Menu...";
    cin.ignore();
    cin.get();
}

void completePendingPayment(int orderId) {
    clearScreen();
    displayBanner();
    cout << "==================== COMPLETE PAYMENT ====================\n";

    // Fetch the total amount for the order
    string query = "SELECT TotalAmount FROM orders WHERE OrderID = " + to_string(orderId);
    MYSQL_RES* res = executeSelectQuery(query);
    MYSQL_ROW row = mysql_fetch_row(res);
    double totalAmount = 0.0;
    if (row) {
        totalAmount = stod(row[0]);
    }
    mysql_free_result(res);

    query = "SELECT Balance FROM wallet WHERE UserID = " + currentUserID;
    res = executeSelectQuery(query);

    if (res) {
        row = mysql_fetch_row(res);
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

                    string updateOrder = "UPDATE orders SET OrderStatus = 'Completed' WHERE OrderID = " + to_string(orderId);
                    executeUpdate(updateOrder);

                    cout << "Payment successful! Order completed." << endl;
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
    cout << "\nPress Enter to return to the Pending Orders Menu...";
    cin.ignore();
    cin.get();
}

void cancelOrder(int orderId) {
    clearScreen();
    displayBanner();
    cout << "==================== CANCEL ORDER ====================\n";

    // Fetch the order details
    string query = "SELECT TotalAmount FROM orders WHERE OrderID = " + to_string(orderId);
    MYSQL_RES* res = executeSelectQuery(query);
    MYSQL_ROW row = mysql_fetch_row(res);
    double totalAmount = 0.0;
    if (row) {
        totalAmount = stod(row[0]);
    }
    mysql_free_result(res);

    // Update order status to cancelled
    string updateOrder = "UPDATE orders SET OrderStatus = 'Cancelled' WHERE OrderID = " + to_string(orderId);
    if (executeUpdate(updateOrder)) {
        cout << "Order cancelled successfully!\n";

        // Restore product stock
        query = "SELECT ProductID, Quantity FROM order_items WHERE OrderID = " + to_string(orderId);
        res = executeSelectQuery(query);
        while ((row = mysql_fetch_row(res))) {
            int productID = stoi(row[0]);
            int quantity = stoi(row[1]);
            string updateStock = "UPDATE product SET StockQuantity = StockQuantity + " + to_string(quantity) + " WHERE ProductID = " + to_string(productID);
            executeUpdate(updateStock);
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error cancelling order: " << mysql_error(conn) << endl;
    }

    cout << "=======================================================\n";
    cout << "\nPress Enter to return to the Pending Orders Menu...";
    cin.ignore();
    cin.get();
}

int generateOrder(const vector<int>& selectedItems, double selectedTotalAmount) {
    // Use a valid StatusID (e.g., 1 for 'Pending Seller Approval')
    int validStatusID = 1;

    // Generate a new order and save to the database
    string insertOrderQuery = "INSERT INTO orders (UserID, TotalAmount, StatusID, OrderDate) VALUES (" + currentUserID + ", " + to_string(selectedTotalAmount) + ", " + to_string(validStatusID) + ", NOW())";

    if (executeUpdate(insertOrderQuery)) {
        // Retrieve the last inserted order ID
        string query = "SELECT LAST_INSERT_ID()";
        MYSQL_RES* resLastInsertId = executeSelectQuery(query);
        MYSQL_ROW rowLastInsertId = mysql_fetch_row(resLastInsertId);
        int orderId = stoi(rowLastInsertId[0]);
        mysql_free_result(resLastInsertId);

        // Insert selected items into the order_items table
        for (int productID : selectedItems) {
            query = "SELECT Quantity, TotalPrice / Quantity AS UnitPrice, ProductName, StoreName FROM cart WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
            MYSQL_RES* res = executeSelectQuery(query);
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {
                int quantity = stoi(row[0]);
                double unitPrice = stod(row[1]);
                string productName = row[2];
                string storeName = row[3];

                string itemQuery = "INSERT INTO order_items (OrderID, ProductID, Quantity, UnitPrice, ProductName, StoreName) "
                    "VALUES (" + to_string(orderId) + ", " + to_string(productID) + ", " + to_string(quantity) + ", " + to_string(unitPrice) + ", '" + productName + "', '" + storeName + "')";
                executeUpdate(itemQuery);

                // Remove item from cart after adding to order_items
                string removeCartQuery = "DELETE FROM cart WHERE UserID = " + currentUserID + " AND ProductID = " + to_string(productID);
                executeUpdate(removeCartQuery);
            }
            mysql_free_result(res);
        }

        cout << "Order placed successfully! Your order ID is " << orderId << "." << endl;
        return orderId;
    }
    else {
        cout << "Error placing order: " << mysql_error(conn) << endl;
        return -1;
    }
}


int main() {
    connectToDatabase(); // Establish database connection
    mainMenu(); // Start the main menu
    return 0;
}
