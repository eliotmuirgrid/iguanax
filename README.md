# Opps I nuked the Iguana X Source code Repo - but it's been forked already

I deliberately nuked this archive of Iguana X source code to illustrate a point.  Critical
infra-structure software cannot depend on the whims of single person nor vendor. It's too
damned important to trust that one day the founder won't accept a modest sized check from
[Larry Elison](https://en.wikipedia.org/wiki/Larry_Ellison) and go sip pina coladas on a cruise
ship.

Because Larry will do what Larry does and jack up the price to unreasonable levels because he
wants to make 50X times the modest check he wrote to the founder.  This is how business works
and this is why you need to have better arrangements when it comes 

Iguana X isn't yet fit to be critical infra-structure code for anyone.  The queueing/logging
part of the product while it does have an elegant design doesn't appear to have the features
one needs.  A queueing system shouldn't have these 'features':

- It shouldn't stop mysteriously working.
- It shouldn't mysteriously lose data.
- It shouldn't have have real mysteries.

In general **mysterious behavior** with a queuing system is a bad idea.

Also the product needs to operate completely independently of any cloud resources like bitbucket. It
doesn't yet. 

I will get around to fixing that and there will be a Mozilla 2.0 licensed version of Iguana X
when I have had time to make something which actually works stably in production.  Until that
point I am recommending all my customers use either Iguana 6.1.5 or Iguana 6.2.

Go to [www.interfaceware.com](https://www.interfaceware.com). For more information.
