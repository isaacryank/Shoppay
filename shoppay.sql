-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 21, 2025 at 10:32 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `shoppay`
--

-- --------------------------------------------------------

--
-- Table structure for table `cart`
--

CREATE TABLE `cart` (
  `CartID` int(11) NOT NULL,
  `UserID` int(11) DEFAULT NULL,
  `ProductID` int(11) DEFAULT NULL,
  `Quantity` int(11) NOT NULL,
  `TotalPrice` double NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `coupon`
--

CREATE TABLE `coupon` (
  `CouponID` int(11) NOT NULL,
  `Code` varchar(50) NOT NULL,
  `DiscountPercentage` double NOT NULL,
  `ValidFrom` date NOT NULL,
  `ValidUntil` date NOT NULL,
  `MinPurchase` double NOT NULL,
  `UsageLimit` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `coupon`
--

INSERT INTO `coupon` (`CouponID`, `Code`, `DiscountPercentage`, `ValidFrom`, `ValidUntil`, `MinPurchase`, `UsageLimit`) VALUES
(1, 'DISCOUNT10', 10, '2025-02-23', '2025-03-23', 10, 2);

-- --------------------------------------------------------

--
-- Table structure for table `messages`
--

CREATE TABLE `messages` (
  `MessageID` int(11) NOT NULL,
  `SenderID` int(11) DEFAULT NULL,
  `ReceiverID` int(11) DEFAULT NULL,
  `MessageText` text NOT NULL,
  `Timestamp` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `messages`
--

INSERT INTO `messages` (`MessageID`, `SenderID`, `ReceiverID`, `MessageText`, `Timestamp`) VALUES
(1, 6, 3, 'Hi, I need Help', '2025-01-21 16:03:25'),
(2, 6, 3, 'Still have ?', '2025-01-21 16:23:11');

-- --------------------------------------------------------

--
-- Table structure for table `orders`
--

CREATE TABLE `orders` (
  `OrderID` int(11) NOT NULL,
  `UserID` int(11) DEFAULT NULL,
  `TotalAmount` double NOT NULL,
  `StatusID` int(11) DEFAULT NULL,
  `OrderDate` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `orders`
--

INSERT INTO `orders` (`OrderID`, `UserID`, `TotalAmount`, `StatusID`, `OrderDate`) VALUES
(41, 6, 15, 2, '2025-01-21 19:13:46'),
(42, 6, 48, 2, '2025-01-21 19:27:14'),
(43, 6, 24, 1, '2025-01-21 19:40:22'),
(44, 6, 86, 1, '2025-01-21 19:53:39'),
(45, 6, 21, 2, '2025-01-21 20:07:44'),
(46, 6, 50, 2, '2025-01-21 20:18:40'),
(47, 6, 24, 2, '2025-01-21 20:33:07'),
(48, 6, 15, 2, '2025-01-21 20:35:43'),
(49, 6, 68, 2, '2025-01-21 20:37:31'),
(50, 6, 15, 2, '2025-01-21 20:43:39'),
(51, 6, 260, 1, '2025-01-21 20:58:45');

-- --------------------------------------------------------

--
-- Table structure for table `order_items`
--

CREATE TABLE `order_items` (
  `OrderItemID` int(11) NOT NULL,
  `OrderID` int(11) DEFAULT NULL,
  `ProductID` int(11) DEFAULT NULL,
  `Quantity` int(11) NOT NULL,
  `UnitPrice` double NOT NULL,
  `ProductName` varchar(100) DEFAULT NULL,
  `StoreName` varchar(100) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `order_items`
--

INSERT INTO `order_items` (`OrderItemID`, `OrderID`, `ProductID`, `Quantity`, `UnitPrice`, `ProductName`, `StoreName`) VALUES
(44, 41, 1, 2, 25, 'EARBUDS', 'UGREEN'),
(45, 41, 7, 5, 3, 'CLAY POT', 'DIY'),
(46, 42, 3, 1, 50, 'KEYBOARD', 'UGREEN'),
(47, 42, 8, 2, 24, 'LADDER 7FT', 'DIY'),
(48, 43, 10, 2, 12, 'MINI FAN', 'GOJODOQ'),
(49, 44, 12, 10, 4, 'APT KEYCHAIN', 'UGREEN'),
(50, 44, 5, 1, 13, 'STYLUS PEN', 'UGREEN'),
(51, 44, 7, 3, 3, 'CLAY POT', 'DIY'),
(52, 44, 10, 2, 12, 'MINI FAN', 'GOJODOQ'),
(53, 45, 3, 1, 50, 'KEYBOARD', 'UGREEN'),
(54, 45, 7, 7, 3, 'CLAY POT', 'DIY'),
(55, 46, 1, 2, 25, 'EARBUDS', 'UGREEN'),
(56, 47, 10, 2, 12, 'MINI FAN', 'GOJODOQ'),
(57, 48, 1, 1, 25, 'EARBUDS', 'UGREEN'),
(58, 48, 7, 5, 3, 'CLAY POT', 'DIY'),
(59, 49, 9, 2, 34, 'PORTABLE HANDHELD', 'GOJODOQ'),
(60, 50, 7, 5, 3, 'CLAY POT', 'DIY'),
(61, 50, 10, 2, 12, 'MINI FAN', 'GOJODOQ'),
(62, 51, 1, 2, 25, 'EARBUDS', 'UGREEN'),
(63, 51, 6, 3, 70, 'GRILL X42', 'DIY');

-- --------------------------------------------------------

--
-- Table structure for table `order_status`
--

CREATE TABLE `order_status` (
  `StatusID` int(11) NOT NULL,
  `StatusName` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `order_status`
--

INSERT INTO `order_status` (`StatusID`, `StatusName`) VALUES
(1, 'Pending Seller Approval'),
(2, 'Completed'),
(3, 'Pending Payment'),
(4, 'Cancelled');

-- --------------------------------------------------------

--
-- Table structure for table `product`
--

CREATE TABLE `product` (
  `ProductID` int(11) NOT NULL,
  `SellerID` int(11) DEFAULT NULL,
  `ProductName` varchar(100) NOT NULL,
  `Description` text DEFAULT NULL,
  `Price` double NOT NULL,
  `StockQuantity` int(11) NOT NULL,
  `StoreName` varchar(100) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `product`
--

INSERT INTO `product` (`ProductID`, `SellerID`, `ProductName`, `Description`, `Price`, `StockQuantity`, `StoreName`) VALUES
(1, 3, 'EARBUDS', 'MODEL TYS 110', 25, 3, 'UGREEN'),
(3, 3, 'KEYBOARD', 'GREEN SWITCH', 50, 50, 'UGREEN'),
(5, 3, 'STYLUS PEN', 'COMPATIBLE WITH ALL DEVICE. BLUETOOTH 5.0v.', 13, 51, 'UGREEN'),
(6, 4, 'GRILL X42', 'COMPLETE SET WITH ADDITIONAL TOOLS', 70, 30, 'DIY'),
(7, 4, 'CLAY POT', 'BRING NATURE TO YOUR HOUSE', 3, 75, 'DIY'),
(8, 4, 'LADDER 7FT', 'NEW DESIGN WITH GREATER STRENGTH', 24, 78, 'DIY'),
(9, 7, 'PORTABLE HANDHELD', 'TURBO FAN 100 WIND', 34, 82, 'GOJODOQ'),
(10, 7, 'MINI FAN', '3000mAH USB CLIP FAN ROTATING', 12, 5, 'GOJODOQ'),
(11, 7, 'MINI POWERBANK', '10000mAH HOT PORTABLE WIRESLESS', 89, 4, 'GOJODOQ'),
(12, 3, 'APT KEYCHAIN', 'AT', 4, 190, 'UGREEN');

-- --------------------------------------------------------

--
-- Table structure for table `system_settings`
--

CREATE TABLE `system_settings` (
  `SettingID` int(11) NOT NULL,
  `SettingName` varchar(50) NOT NULL,
  `Value` double NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `system_settings`
--

INSERT INTO `system_settings` (`SettingID`, `SettingName`, `Value`) VALUES
(1, 'CommissionRate', 0.05);

-- --------------------------------------------------------

--
-- Table structure for table `transaction`
--

CREATE TABLE `transaction` (
  `TransactionID` int(11) NOT NULL,
  `WalletID` int(11) DEFAULT NULL,
  `Amount` double NOT NULL,
  `TransactionType` varchar(50) NOT NULL,
  `TransactionDate` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

CREATE TABLE `user` (
  `UserID` int(11) NOT NULL,
  `Username` varchar(50) NOT NULL,
  `Email` varchar(100) NOT NULL,
  `Password` varchar(100) NOT NULL,
  `FullName` varchar(100) DEFAULT NULL,
  `Address` varchar(255) DEFAULT NULL,
  `PhoneNumber` varchar(20) DEFAULT NULL,
  `UserRole` int(11) NOT NULL,
  `isApproved` tinyint(1) DEFAULT 0,
  `StoreName` varchar(100) DEFAULT NULL,
  `RegistrationDate` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `user`
--

INSERT INTO `user` (`UserID`, `Username`, `Email`, `Password`, `FullName`, `Address`, `PhoneNumber`, `UserRole`, `isApproved`, `StoreName`, `RegistrationDate`) VALUES
(1, 'Admin', 'Admin1@gmail.com', 'hashed_123', 'Isaac [Admin]', '', '014 690 5325', 2, 0, '', '2025-01-19 03:15:17'),
(2, 'Admin2', 'Admin2@gmail.com', 'hashed_123', 'Mirul [Admin]', '', '013 233 4455', 2, 0, '', '2025-01-19 03:17:42'),
(3, 'Seller', 'Seller1@gmail.com', 'hashed_123', 'Abdul Kazim', '', '013 232 3344', 1, 1, 'UGREEN', '2025-01-19 03:18:28'),
(4, 'Seller2', 'seller2', 'hashed_123', 'Tan Cheng', 'UTeM', '013 445 3232', 1, 1, 'DIY', '2025-01-19 03:19:42'),
(5, 'Veron', 'veron@gmail.com', 'hashed_123', 'Veron Jr', '', '012 332 2345', 0, 0, '', '2025-01-19 04:31:41'),
(6, 'Isaac', 'isaac', 'hashed_123', 'Isaac Ryan', 'Taman Tambun, Melaka', '014 225 3485', 0, 0, '', '2025-01-19 04:32:24'),
(7, 'Seller3', 'seller3@gmail.com', 'hashed_123', 'Anis Hazuki', '', '012 323 4343', 1, 1, 'GOJODOQ', '2025-01-19 07:23:27'),
(8, 'Seller4', 'seller@gmail.com', 'hashed_', 'Seller4', '', '123 ', 1, 0, 'ECO SHOP', '2025-01-21 15:47:48'),
(10, 'Seller5', 'sellr@gmail.com', 'hashed_123', 'Seller5', '', '122', 1, 0, '', '2025-01-21 15:49:58');

-- --------------------------------------------------------

--
-- Table structure for table `wallet`
--

CREATE TABLE `wallet` (
  `WalletID` int(11) NOT NULL,
  `UserID` int(11) DEFAULT NULL,
  `Balance` double NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `wallet`
--

INSERT INTO `wallet` (`WalletID`, `UserID`, `Balance`) VALUES
(1, 6, 6434),
(2, 3, 2162),
(3, 4, 1702),
(4, 7, 138);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `cart`
--
ALTER TABLE `cart`
  ADD PRIMARY KEY (`CartID`),
  ADD KEY `UserID` (`UserID`),
  ADD KEY `ProductID` (`ProductID`);

--
-- Indexes for table `coupon`
--
ALTER TABLE `coupon`
  ADD PRIMARY KEY (`CouponID`),
  ADD UNIQUE KEY `Code` (`Code`);

--
-- Indexes for table `messages`
--
ALTER TABLE `messages`
  ADD PRIMARY KEY (`MessageID`),
  ADD KEY `SenderID` (`SenderID`),
  ADD KEY `ReceiverID` (`ReceiverID`);

--
-- Indexes for table `orders`
--
ALTER TABLE `orders`
  ADD PRIMARY KEY (`OrderID`),
  ADD KEY `UserID` (`UserID`);

--
-- Indexes for table `order_items`
--
ALTER TABLE `order_items`
  ADD PRIMARY KEY (`OrderItemID`),
  ADD KEY `OrderID` (`OrderID`),
  ADD KEY `ProductID` (`ProductID`);

--
-- Indexes for table `order_status`
--
ALTER TABLE `order_status`
  ADD PRIMARY KEY (`StatusID`);

--
-- Indexes for table `product`
--
ALTER TABLE `product`
  ADD PRIMARY KEY (`ProductID`),
  ADD KEY `SellerID` (`SellerID`);

--
-- Indexes for table `system_settings`
--
ALTER TABLE `system_settings`
  ADD PRIMARY KEY (`SettingID`),
  ADD UNIQUE KEY `SettingName` (`SettingName`);

--
-- Indexes for table `transaction`
--
ALTER TABLE `transaction`
  ADD PRIMARY KEY (`TransactionID`),
  ADD KEY `WalletID` (`WalletID`);

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`UserID`),
  ADD UNIQUE KEY `Username` (`Username`),
  ADD UNIQUE KEY `Email` (`Email`);

--
-- Indexes for table `wallet`
--
ALTER TABLE `wallet`
  ADD PRIMARY KEY (`WalletID`),
  ADD KEY `UserID` (`UserID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `cart`
--
ALTER TABLE `cart`
  MODIFY `CartID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=65;

--
-- AUTO_INCREMENT for table `coupon`
--
ALTER TABLE `coupon`
  MODIFY `CouponID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `messages`
--
ALTER TABLE `messages`
  MODIFY `MessageID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `orders`
--
ALTER TABLE `orders`
  MODIFY `OrderID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=52;

--
-- AUTO_INCREMENT for table `order_items`
--
ALTER TABLE `order_items`
  MODIFY `OrderItemID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=64;

--
-- AUTO_INCREMENT for table `order_status`
--
ALTER TABLE `order_status`
  MODIFY `StatusID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `product`
--
ALTER TABLE `product`
  MODIFY `ProductID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=14;

--
-- AUTO_INCREMENT for table `system_settings`
--
ALTER TABLE `system_settings`
  MODIFY `SettingID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `transaction`
--
ALTER TABLE `transaction`
  MODIFY `TransactionID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `user`
--
ALTER TABLE `user`
  MODIFY `UserID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=11;

--
-- AUTO_INCREMENT for table `wallet`
--
ALTER TABLE `wallet`
  MODIFY `WalletID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `cart`
--
ALTER TABLE `cart`
  ADD CONSTRAINT `cart_ibfk_1` FOREIGN KEY (`UserID`) REFERENCES `user` (`UserID`),
  ADD CONSTRAINT `cart_ibfk_2` FOREIGN KEY (`ProductID`) REFERENCES `product` (`ProductID`);

--
-- Constraints for table `messages`
--
ALTER TABLE `messages`
  ADD CONSTRAINT `messages_ibfk_1` FOREIGN KEY (`SenderID`) REFERENCES `user` (`UserID`),
  ADD CONSTRAINT `messages_ibfk_2` FOREIGN KEY (`ReceiverID`) REFERENCES `user` (`UserID`);

--
-- Constraints for table `orders`
--
ALTER TABLE `orders`
  ADD CONSTRAINT `orders_ibfk_1` FOREIGN KEY (`UserID`) REFERENCES `user` (`UserID`);

--
-- Constraints for table `order_items`
--
ALTER TABLE `order_items`
  ADD CONSTRAINT `order_items_ibfk_1` FOREIGN KEY (`OrderID`) REFERENCES `orders` (`OrderID`),
  ADD CONSTRAINT `order_items_ibfk_2` FOREIGN KEY (`ProductID`) REFERENCES `product` (`ProductID`);

--
-- Constraints for table `product`
--
ALTER TABLE `product`
  ADD CONSTRAINT `product_ibfk_1` FOREIGN KEY (`SellerID`) REFERENCES `user` (`UserID`);

--
-- Constraints for table `transaction`
--
ALTER TABLE `transaction`
  ADD CONSTRAINT `transaction_ibfk_1` FOREIGN KEY (`WalletID`) REFERENCES `wallet` (`WalletID`);

--
-- Constraints for table `wallet`
--
ALTER TABLE `wallet`
  ADD CONSTRAINT `wallet_ibfk_1` FOREIGN KEY (`UserID`) REFERENCES `user` (`UserID`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
