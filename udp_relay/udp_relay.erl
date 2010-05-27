-module(udp_relay).
-compile(export_all).

relay(Port1, Port2)->
    {ok, Socket} = gen_udp:open(Port1, [binary]),
    {ok, Socket_send} = gen_udp:open(0, [binary]),
    loop(Socket,Socket_send, Port2).

relay2(Port1, Port2)->
    {ok, Socket} = gen_udp:open(Port1, [binary]),
    {ok, Socket_send} = gen_udp:open(0, [binary]),
    recv_send(Socket, Socket_send, Port2).

recv_send(Socket, Socket_send, Port2) ->
    receive
        {udp, Socket, _Host, _Port, Bin} ->
            spawn(gen_udp, send, [Socket_send, "localhost", Port2, Bin]),
            recv_send(Socket, Socket_send, Port2)
    end.

loop(Socket, Socket_send, Port2)->
    receive
        {udp, Socket, _Host, _Port, Bin} ->
	    ok = gen_udp:send(Socket_send, "localhost", Port2, Bin),
	    loop(Socket, Socket_send, Port2)
    end.

server(Port)->
    {ok, Socket} = gen_udp:open(0, [binary]),
    loop_server(Socket, Port, 0).

loop_server(Socket, Port, Number)->
    receive
    after
        1 -> gen_udp:send(Socket, "localhost", Port, term_to_binary(Number)),
            loop_server(Socket, Port, Number+1)
    end.

client(Port)->
    {ok, Socket} = gen_udp:open(Port, [binary]),
    loop_client(Socket).

loop_client(Socket)->
    receive
        {udp, Socket, _Host, _Port, Bin} ->
            io:format("~w~n", [binary_to_term(Bin)]),
            loop_client(Socket)
    end.

test ()->
    relay(10000, 10020).

test2() -> 
    relay2(10000, 10020).
