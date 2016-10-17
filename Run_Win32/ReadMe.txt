_______          __                       __   .__
\      \   _____/  |___  _  _____________|  | _|__| ____    ____
/   |   \_/ __ \   __\ \/ \/ /  _ \_  __ \  |/ /  |/    \  / ___\
/    |    \  ___/|  |  \     (  <_> )  | \/    <|  |   |  \/ /_/  >
\____|__  /\___  >__|   \/\_/ \____/|__|  |__|_ \__|___|  /\___  /
       \/     \/                              \/       \//_____/
 _________
/   _____/_  _  _______     ____
\_____  \\ \/ \/ /\__  \   / ___\
/        \\     /  / __ \_/ /_/  >
/_______  / \/\_/  (____  /\___  /
       \/              \//_____/

How to test:
  When the game starts, we automatically run nsstart to start the netsession.
  Start up the remote command server by running cshost on one and csjoin on the other.
  Then, run bcmd, or broadcast command, with nscreateconn
    EX: "bcmd nscreateconn 0 right 192.168.0.4:4334"
  Run nsdebug, which will run the debugging tools in the console.

  You can now test my reliable message: hb (Yes, I understand that this isn't actually a heartbeat bear with my sleeplessness)
    EX: hb 192.168.0.4:4334
  The command will send a reliable message to the recipient, which will show the reliable id received in brackets on the printout

  If you'd like to run multiple heartbeats:
    runfor <numberOfHeartbeats> hb 192.168.0.4:4334

Known Issues:
  - Note that I'm currently sending packets to myself to enable you to test without a partner and for debug reasons, this obviously isn't what I should do in real life
  - I think I meet all the core requirements, but I'm uncertain if there are some latent issues with what I've implemented that I don't understand.
  I'd like to meet up with you and make sure everything is okay at a later date.
