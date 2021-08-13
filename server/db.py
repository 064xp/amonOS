def initDatabase(conn):
    cur = conn.cursor()
    # Create table
    cur.execute('''CREATE TABLE IF NOT EXISTS responses (
        user text,
        cwd text,
        buffer text,
        secCode integer )
    ''')

    cur.execute('''CREATE TABLE IF NOT EXISTS requests (
        user text,
        secCode integer primary key autoincrement)
    ''')

    conn.commit()

def insertRequest(conn, user):
    cur = conn.cursor()
    query = 'INSERT INTO requests (user) VALUES (?)'
    cur.execute(query, (user,))
    conn.commit()

    return cur.lastrowid

def insertResponse(conn, user, cwd, buffer, secCode):
    cur = conn.cursor()
    query = 'INSERT INTO responses VALUES (?, ?, ?, ?)'
    cur.execute(query, (user, cwd, buffer, secCode))
    conn.commit()

    return cur.lastrowid


def getResponse(conn, secCode):
    cur = conn.cursor()
    query = 'SELECT * FROM responses WHERE secCode = ?'
    cur.execute(query, (secCode,))

    return cur.fetchone()

def requestCleanup(conn, secCode):
    cur = conn.cursor()
    query = 'DELETE FROM responses WHERE secCode = ?'
    cur.execute(query, (secCode,))

    query = 'DELETE FROM  requests WHERE secCode = ?'
    cur.execute(query, (secCode,))

    conn.commit()

def getAllResponses(conn):
    cur = conn.cursor()
    query = 'SELECT * FROM responses'
    cur.execute(query)

    return cur.fetchall()
