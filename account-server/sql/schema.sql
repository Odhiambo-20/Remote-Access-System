-- Remote Access System Database Schema
-- SQLite3 Database Schema for Account Server
-- Version: 1.0

-- Enable foreign keys
PRAGMA foreign_keys = ON;

-- Set journal mode to WAL for better concurrency
PRAGMA journal_mode = WAL;

-- Set synchronous mode for better performance
PRAGMA synchronous = NORMAL;

-- =============================================================================
-- SCHEMA VERSION TABLE
-- =============================================================================
CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER PRIMARY KEY,
    applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

-- Insert initial schema version
INSERT OR IGNORE INTO schema_version (version, description) 
VALUES (1, 'Initial schema creation');

-- =============================================================================
-- USERS TABLE
-- Store user account information
-- =============================================================================
CREATE TABLE IF NOT EXISTS users (
    username VARCHAR(64) PRIMARY KEY,
    password_hash VARCHAR(256) NOT NULL,
    salt VARCHAR(64) NOT NULL,
    email VARCHAR(128),
    full_name VARCHAR(128),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    account_status VARCHAR(20) DEFAULT 'active' CHECK(account_status IN ('active', 'suspended', 'deleted')),
    failed_login_attempts INTEGER DEFAULT 0,
    locked_until TIMESTAMP
);

-- Create indexes for users table
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_users_status ON users(account_status);
CREATE INDEX IF NOT EXISTS idx_users_last_login ON users(last_login);

-- =============================================================================
-- PCS TABLE
-- Store PC/Computer registration information
-- =============================================================================
CREATE TABLE IF NOT EXISTS pcs (
    pc_id VARCHAR(128) PRIMARY KEY,
    username VARCHAR(64) NOT NULL,
    pc_name VARCHAR(128),
    hostname VARCHAR(128),
    os_type VARCHAR(32),
    os_version VARCHAR(64),
    ipv4_address VARCHAR(45),
    ipv6_address VARCHAR(45),
    relay_endpoint VARCHAR(128),
    last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    online BOOLEAN DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
);

-- Create indexes for pcs table
CREATE INDEX IF NOT EXISTS idx_pcs_username ON pcs(username);
CREATE INDEX IF NOT EXISTS idx_pcs_online ON pcs(online);
CREATE INDEX IF NOT EXISTS idx_pcs_last_seen ON pcs(last_seen);
CREATE INDEX IF NOT EXISTS idx_pcs_username_online ON pcs(username, online);

-- =============================================================================
-- MOBILE DEVICES TABLE
-- Store mobile device registration information
-- =============================================================================
CREATE TABLE IF NOT EXISTS mobile_devices (
    device_id VARCHAR(128) PRIMARY KEY,
    username VARCHAR(64) NOT NULL,
    device_name VARCHAR(128),
    device_model VARCHAR(128),
    device_os VARCHAR(32),
    device_os_version VARCHAR(64),
    app_version VARCHAR(32),
    push_token VARCHAR(256),
    last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
);

-- Create indexes for mobile_devices table
CREATE INDEX IF NOT EXISTS idx_mobile_username ON mobile_devices(username);
CREATE INDEX IF NOT EXISTS idx_mobile_last_seen ON mobile_devices(last_seen);

-- =============================================================================
-- DEVICE BINDINGS TABLE
-- Store relationships between mobile devices and PCs
-- =============================================================================
CREATE TABLE IF NOT EXISTS device_bindings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id VARCHAR(128) NOT NULL,
    pc_id VARCHAR(128) NOT NULL,
    bound_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    bound_by VARCHAR(64),
    nickname VARCHAR(128),
    UNIQUE(device_id, pc_id),
    FOREIGN KEY (device_id) REFERENCES mobile_devices(device_id) ON DELETE CASCADE,
    FOREIGN KEY (pc_id) REFERENCES pcs(pc_id) ON DELETE CASCADE,
    FOREIGN KEY (bound_by) REFERENCES users(username) ON DELETE SET NULL
);

-- Create indexes for device_bindings table
CREATE INDEX IF NOT EXISTS idx_bindings_device ON device_bindings(device_id);
CREATE INDEX IF NOT EXISTS idx_bindings_pc ON device_bindings(pc_id);
CREATE INDEX IF NOT EXISTS idx_bindings_bound_at ON device_bindings(bound_at);

-- =============================================================================
-- SESSIONS TABLE
-- Store active user sessions
-- =============================================================================
CREATE TABLE IF NOT EXISTS sessions (
    session_token VARCHAR(128) PRIMARY KEY,
    username VARCHAR(64) NOT NULL,
    device_id VARCHAR(128) NOT NULL,
    device_type VARCHAR(20) NOT NULL CHECK(device_type IN ('PC', 'MOBILE', 'WEB')),
    ip_address VARCHAR(45),
    user_agent TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_activity TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP NOT NULL,
    FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
);

-- Create indexes for sessions table
CREATE INDEX IF NOT EXISTS idx_sessions_username ON sessions(username);
CREATE INDEX IF NOT EXISTS idx_sessions_device ON sessions(device_id);
CREATE INDEX IF NOT EXISTS idx_sessions_created ON sessions(created_at);
CREATE INDEX IF NOT EXISTS idx_sessions_expires ON sessions(expires_at);
CREATE INDEX IF NOT EXISTS idx_sessions_last_activity ON sessions(last_activity);

-- =============================================================================
-- AUDIT LOG TABLE
-- Store audit trail of important events
-- =============================================================================
CREATE TABLE IF NOT EXISTS audit_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    username VARCHAR(64),
    event_type VARCHAR(50) NOT NULL,
    event_description TEXT,
    ip_address VARCHAR(45),
    device_id VARCHAR(128),
    success BOOLEAN DEFAULT 1,
    error_message TEXT,
    FOREIGN KEY (username) REFERENCES users(username) ON DELETE SET NULL
);

-- Create indexes for audit_log table
CREATE INDEX IF NOT EXISTS idx_audit_timestamp ON audit_log(timestamp);
CREATE INDEX IF NOT EXISTS idx_audit_username ON audit_log(username);
CREATE INDEX IF NOT EXISTS idx_audit_event_type ON audit_log(event_type);
CREATE INDEX IF NOT EXISTS idx_audit_success ON audit_log(success);

-- =============================================================================
-- FILE SHARES TABLE
-- Store information about shared files
-- =============================================================================
CREATE TABLE IF NOT EXISTS file_shares (
    share_id VARCHAR(128) PRIMARY KEY,
    pc_id VARCHAR(128) NOT NULL,
    username VARCHAR(64) NOT NULL,
    file_path TEXT NOT NULL,
    file_name VARCHAR(256) NOT NULL,
    file_size BIGINT,
    share_url TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP NOT NULL,
    access_count INTEGER DEFAULT 0,
    max_access_count INTEGER DEFAULT -1,
    password_protected BOOLEAN DEFAULT 0,
    password_hash VARCHAR(256),
    FOREIGN KEY (pc_id) REFERENCES pcs(pc_id) ON DELETE CASCADE,
    FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
);

-- Create indexes for file_shares table
CREATE INDEX IF NOT EXISTS idx_shares_pc ON file_shares(pc_id);
CREATE INDEX IF NOT EXISTS idx_shares_username ON file_shares(username);
CREATE INDEX IF NOT EXISTS idx_shares_expires ON file_shares(expires_at);
CREATE INDEX IF NOT EXISTS idx_shares_created ON file_shares(created_at);

-- =============================================================================
-- STATISTICS TABLE
-- Store server statistics and metrics
-- =============================================================================
CREATE TABLE IF NOT EXISTS statistics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    stat_date DATE DEFAULT (date('now')),
    stat_hour INTEGER DEFAULT (strftime('%H', 'now')),
    metric_name VARCHAR(64) NOT NULL,
    metric_value INTEGER DEFAULT 0,
    UNIQUE(stat_date, stat_hour, metric_name)
);

-- Create indexes for statistics table
CREATE INDEX IF NOT EXISTS idx_stats_date ON statistics(stat_date);
CREATE INDEX IF NOT EXISTS idx_stats_metric ON statistics(metric_name);

-- =============================================================================
-- VIEWS
-- Convenient views for common queries
-- =============================================================================

-- View: User PC Summary
CREATE VIEW IF NOT EXISTS v_user_pc_summary AS
SELECT 
    u.username,
    u.full_name,
    COUNT(p.pc_id) AS total_pcs,
    SUM(CASE WHEN p.online = 1 THEN 1 ELSE 0 END) AS online_pcs,
    MAX(p.last_seen) AS last_pc_activity
FROM users u
LEFT JOIN pcs p ON u.username = p.username
GROUP BY u.username;

-- View: Active Sessions Summary
CREATE VIEW IF NOT EXISTS v_active_sessions AS
SELECT 
    s.session_token,
    s.username,
    s.device_id,
    s.device_type,
    s.created_at,
    s.last_activity,
    s.expires_at,
    (julianday(s.expires_at) - julianday('now')) * 24 * 60 AS minutes_until_expire
FROM sessions s
WHERE s.expires_at > datetime('now')
ORDER BY s.last_activity DESC;

-- View: Device Binding Summary
CREATE VIEW IF NOT EXISTS v_binding_summary AS
SELECT 
    md.device_id,
    md.device_name,
    md.username,
    COUNT(db.pc_id) AS bound_pc_count,
    GROUP_CONCAT(p.pc_name, ', ') AS bound_pcs
FROM mobile_devices md
LEFT JOIN device_bindings db ON md.device_id = db.device_id
LEFT JOIN pcs p ON db.pc_id = p.pc_id
GROUP BY md.device_id;

-- View: Recent Audit Events
CREATE VIEW IF NOT EXISTS v_recent_audit AS
SELECT 
    al.timestamp,
    al.username,
    al.event_type,
    al.event_description,
    al.success,
    al.ip_address
FROM audit_log al
ORDER BY al.timestamp DESC
LIMIT 1000;

-- =============================================================================
-- TRIGGERS
-- Automatic updates and integrity checks
-- =============================================================================

-- Trigger: Update users.updated_at on modification
CREATE TRIGGER IF NOT EXISTS trg_users_updated
AFTER UPDATE ON users
BEGIN
    UPDATE users SET updated_at = CURRENT_TIMESTAMP WHERE username = NEW.username;
END;

-- Trigger: Update pcs.updated_at on modification
CREATE TRIGGER IF NOT EXISTS trg_pcs_updated
AFTER UPDATE ON pcs
BEGIN
    UPDATE pcs SET updated_at = CURRENT_TIMESTAMP WHERE pc_id = NEW.pc_id;
END;

-- Trigger: Update mobile_devices.updated_at on modification
CREATE TRIGGER IF NOT EXISTS trg_mobile_updated
AFTER UPDATE ON mobile_devices
BEGIN
    UPDATE mobile_devices SET updated_at = CURRENT_TIMESTAMP WHERE device_id = NEW.device_id;
END;

-- Trigger: Log user creation
CREATE TRIGGER IF NOT EXISTS trg_log_user_create
AFTER INSERT ON users
BEGIN
    INSERT INTO audit_log (username, event_type, event_description, success)
    VALUES (NEW.username, 'USER_CREATED', 'New user account created', 1);
END;

-- Trigger: Log user deletion
CREATE TRIGGER IF NOT EXISTS trg_log_user_delete
BEFORE DELETE ON users
BEGIN
    INSERT INTO audit_log (username, event_type, event_description, success)
    VALUES (OLD.username, 'USER_DELETED', 'User account deleted', 1);
END;

-- Trigger: Log PC registration
CREATE TRIGGER IF NOT EXISTS trg_log_pc_create
AFTER INSERT ON pcs
BEGIN
    INSERT INTO audit_log (username, event_type, event_description, device_id, success)
    VALUES (NEW.username, 'PC_REGISTERED', 'PC registered: ' || NEW.pc_name, NEW.pc_id, 1);
END;

-- Trigger: Log device binding
CREATE TRIGGER IF NOT EXISTS trg_log_binding_create
AFTER INSERT ON device_bindings
BEGIN
    INSERT INTO audit_log (username, event_type, event_description, device_id, success)
    VALUES (NEW.bound_by, 'DEVICE_BOUND', 'Device bound to PC', NEW.device_id, 1);
END;

-- Trigger: Clean up expired sessions daily
CREATE TRIGGER IF NOT EXISTS trg_clean_expired_sessions
AFTER INSERT ON statistics
WHEN NEW.metric_name = 'daily_cleanup'
BEGIN
    DELETE FROM sessions WHERE expires_at < datetime('now');
END;

-- =============================================================================
-- INITIAL DATA
-- Insert default configuration and sample data
-- =============================================================================

-- Insert initial statistics entry to track server start
INSERT OR IGNORE INTO statistics (stat_date, stat_hour, metric_name, metric_value)
VALUES (date('now'), strftime('%H', 'now'), 'server_initialized', 1);

-- =============================================================================
-- DATABASE MAINTENANCE PROCEDURES
-- These are comments for manual maintenance tasks
-- =============================================================================

-- To vacuum the database (compact and optimize):
-- VACUUM;

-- To analyze tables for query optimization:
-- ANALYZE;

-- To check database integrity:
-- PRAGMA integrity_check;

-- To get database size:
-- SELECT page_count * page_size as size FROM pragma_page_count(), pragma_page_size();

-- To backup database:
-- .backup '/path/to/backup.db'

-- To restore database:
-- .restore '/path/to/backup.db'

-- =============================================================================
-- NOTES
-- =============================================================================
-- 1. This schema uses SQLite3 features
-- 2. All timestamps are stored in UTC
-- 3. Foreign keys are enabled and enforced
-- 4. WAL mode is enabled for better concurrency
-- 5. Indexes are created for common query patterns
-- 6. Triggers maintain audit trail automatically
-- 7. Views provide convenient access to common queries
-- 8. Schema version tracking allows for upgrades

-- =============================================================================
-- END OF SCHEMA
-- =============================================================================