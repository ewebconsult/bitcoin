#include <string>
#include <vector>
#include <iostream>
using namespace std;

#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/test/unit_test.hpp>

#include "script/bitcoinconsensus.h"
#include "script/interpreter.h"
#include "utilstrencodings.h"
#include "streams.h"
#include "version.h"
#include "core_io.h"

static const unsigned int flags = SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC;

static std::map<string, unsigned int> mapFlagNames = boost::assign::map_list_of
(string("NONE"), (unsigned int)SCRIPT_VERIFY_NONE)
(string("P2SH"), (unsigned int)SCRIPT_VERIFY_P2SH)
(string("STRICTENC"), (unsigned int)SCRIPT_VERIFY_STRICTENC)
(string("DERSIG"), (unsigned int)SCRIPT_VERIFY_DERSIG)
(string("LOW_S"), (unsigned int)SCRIPT_VERIFY_LOW_S)
(string("SIGPUSHONLY"), (unsigned int)SCRIPT_VERIFY_SIGPUSHONLY)
(string("MINIMALDATA"), (unsigned int)SCRIPT_VERIFY_MINIMALDATA)
(string("NULLDUMMY"), (unsigned int)SCRIPT_VERIFY_NULLDUMMY)
(string("DISCOURAGE_UPGRADABLE_NOPS"), (unsigned int)SCRIPT_VERIFY_DISCOURAGE_UPGRADABLE_NOPS)
(string("CLEANSTACK"), (unsigned int)SCRIPT_VERIFY_CLEANSTACK);

unsigned int ParseScriptFlags(string strFlags)
{
	if (strFlags.empty()) {
		return 0;
	}
	unsigned int flags = 0;
	vector<string> words;
	boost::algorithm::split(words, strFlags, boost::algorithm::is_any_of(","));

	BOOST_FOREACH(string word, words)
	{
		if (!mapFlagNames.count(word))
			BOOST_ERROR("Bad test: unknown verification flag '" << word << "'");
		flags |= mapFlagNames[word];
	}

	return flags;
}

CMutableTransaction BuildCreditingTransaction(const CScript& scriptPubKey)
{
	CMutableTransaction txCredit;
	txCredit.nVersion = 1;
	txCredit.nLockTime = 0;
	txCredit.vin.resize(1);
	txCredit.vout.resize(1);
	txCredit.vin[0].prevout.SetNull();
	txCredit.vin[0].scriptSig = CScript() << CScriptNum(0) << CScriptNum(0);
	txCredit.vin[0].nSequence = std::numeric_limits<unsigned int>::max();
	txCredit.vout[0].scriptPubKey = scriptPubKey;
	txCredit.vout[0].nValue = 0;

	return txCredit;
}

CMutableTransaction BuildSpendingTransaction(const CScript& scriptSig, const CMutableTransaction& txCredit)
{
	CMutableTransaction txSpend;
	txSpend.nVersion = 1;
	txSpend.nLockTime = 0;
	txSpend.vin.resize(1);
	txSpend.vout.resize(1);
	txSpend.vin[0].prevout.hash = txCredit.GetHash();
	txSpend.vin[0].prevout.n = 0;
	txSpend.vin[0].scriptSig = scriptSig;
	txSpend.vin[0].nSequence = std::numeric_limits<unsigned int>::max();
	txSpend.vout[0].scriptPubKey = CScript();
	txSpend.vout[0].nValue = 0;

	return txSpend;
}

void DoTest(const CScript& scriptPubKey, const CScript& scriptSig, int flags, bool expect, const std::string& message)
{
	CMutableTransaction tx = BuildSpendingTransaction(scriptSig, BuildCreditingTransaction(scriptPubKey));
	CMutableTransaction tx2 = tx;
	CDataStream stream(SER_NETWORK, PROTOCOL_VERSION);
	stream << tx2;
	bitcoinconsensus_error err;

	bool result = bitcoinconsensus_verify_script(begin_ptr(scriptPubKey), scriptPubKey.size(), (const unsigned char*)&stream[0], stream.size(), 0, flags, &err);
	assert(result == expect);
}

int main(int argc, char* argv[])
{
	//"1 0x01 0xb9", "HASH160 0x14 0x15727299b05b45fdaf9ac9ecf7565cfe27c3e567 EQUAL", "P2SH,DISCOURAGE_UPGRADABLE_NOPS", "Discouraged NOP10 in redeemScript"
	string scriptSigString = "";
	CScript scriptSig = ParseScript(scriptSigString);
	string scriptPubKeyString = "NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG 0 0 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' 'p' 'q' 'r' 's' 't' 20 CHECKMULTISIG";
	CScript scriptPubKey = ParseScript(scriptPubKeyString);
	unsigned int scriptflags = ParseScriptFlags("P2SH,STRICTENC");

	DoTest(scriptPubKey, scriptSig, scriptflags, false, "");

	return 0;
}
