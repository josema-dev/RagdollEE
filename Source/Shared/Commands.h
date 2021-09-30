/******************************************************************************/
extern const Str CSWelcomeMessage;
/******************************************************************************/
void ClientSendLogin(Connection &conn, C Str &email, C Str &pass);
void ServerReceiveLogin(File &f, Str &email, Str &pass);
void ClientSendPosition(Connection &conn, C Vec2 &pos);
void ServerReceivePosition(File &f, Vec2 &pos);
void ServerWritePosition(File &f, C SockAddr &addr, C Vec2 &pos);
void ClientReceivePosition(File &f, SockAddr &addr, Vec2 &pos);
void ServerSendDelNeighbor(Connection &conn, C SockAddr &addr);
void ClientReceiveDelNeighbor(File &f, SockAddr &addr);
void ServerSendAddNeighbor(Connection &conn, ClientInfo &ci);
void ClientReceiveAddNeighbor(File &f, ClientInfo &ci);
/******************************************************************************/
