def initDatabase(conn):
    cur = conn.cursor()
    # Create table
    cur.execute('CREATE TABLE IF NOT EXISTS responses (user text, cwd text, buffer text, secCode integer)')
    # Save (commit) the changes
    conn.commit()

def insertResponse(conn, user, cwd, buffer, secCode):
    cur = conn.cursor()
    query = 'INSERT INTO responses VALUES (?, ?, ?, ?)'
    cur.execute(query, (user, cwd, buffer, secCode))
    conn.commit()


def getResponse(conn, user):
    cur = conn.cursor()
    query = 'SELECT * FROM responses WHERE user = ?'
    cur.execute(query, (user,))

    return cur.fetchone()

def deleteResponse(conn, user):
    cur = conn.cursor()
    query = 'DELETE FROM responses WHERE user = ?'
    cur.execute(query, (user,))
    conn.commit()

def getAllResponses(conn):
    cur = conn.cursor()
    query = 'SELECT * FROM responses'
    cur.execute(query)

    return cur.fetchall()
