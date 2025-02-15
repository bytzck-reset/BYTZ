// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2021 The Dash Core developers
// Copyright (c) 2021 The Bytz Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <consensus/merkle.h>

#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>

#include <arith_uint256.h>

#include <assert.h>

#include <chainparamsseeds.h>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateDevNetGenesisBlock(const uint256 &prevBlockHash, const std::string& devNetName, uint32_t nTime, uint32_t nNonce, uint32_t nBits, const CAmount& genesisReward)
{
    assert(!devNetName.empty());

    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    // put height (BIP34) and devnet name into coinbase
    txNew.vin[0].scriptSig = CScript() << 1 << std::vector<unsigned char>(devNetName.begin(), devNetName.end());
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = CScript() << OP_RETURN;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = 4;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock = prevBlockHash;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
 *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
 *   vMerkleTree: e0028e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Investing.com 23/Apr/2018 Facebook Gets First Downgrade Since Data Scandal";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}


void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int64_t nWindowSize, int64_t nThresholdStart, int64_t nThresholdMin, int64_t nFalloffCoeff)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
    if (nWindowSize != -1) {
            consensus.vDeployments[d].nWindowSize = nWindowSize;
    }
    if (nThresholdStart != -1) {
        consensus.vDeployments[d].nThresholdStart = nThresholdStart;
    }
    if (nThresholdMin != -1) {
        consensus.vDeployments[d].nThresholdMin = nThresholdMin;
    }
    if (nFalloffCoeff != -1) {
        consensus.vDeployments[d].nFalloffCoeff = nFalloffCoeff;
    }
}

void CChainParams::UpdateDIP3Parameters(int nActivationHeight, int nEnforcementHeight)
{
    consensus.DIP0003Height = nActivationHeight;
//    consensus.DIP0003EnforcementHeight = nEnforcementHeight;
}

void CChainParams::UpdateDIP8Parameters(int nActivationHeight)
{
    consensus.DIP0008Height = nActivationHeight;
}

void CChainParams::UpdateBudgetParameters(int nMasternodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
{
    consensus.nMasternodePaymentsStartBlock = nMasternodePaymentsStartBlock;
    consensus.nBudgetPaymentsStartBlock = nBudgetPaymentsStartBlock;
    consensus.nSuperblockStartBlock = nSuperblockStartBlock;
}

void CChainParams::UpdateSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
{
    consensus.nMinimumDifficultyBlocks = nMinimumDifficultyBlocks;
    consensus.nHighSubsidyBlocks = nHighSubsidyBlocks;
    consensus.nHighSubsidyFactor = nHighSubsidyFactor;
}

void CChainParams::UpdateLLMQChainLocks(Consensus::LLMQType llmqType) {
    consensus.llmqTypeChainLocks = llmqType;
}

void CChainParams::UpdateLLMQInstantSend(Consensus::LLMQType llmqType) {
    consensus.llmqTypeInstantSend = llmqType;
}

void CChainParams::UpdateLLMQTestParams(int size, int threshold) {
    auto& params = consensus.llmqs.at(Consensus::LLMQ_TEST);
    params.size = size;
    params.minSize = threshold;
    params.threshold = threshold;
    params.dkgBadVotesThreshold = threshold;
}

void CChainParams::UpdateLLMQDevnetParams(int size, int threshold)
{
    auto& params = consensus.llmqs.at(Consensus::LLMQ_DEVNET);
    params.size = size;
    params.minSize = threshold;
    params.threshold = threshold;
    params.dkgBadVotesThreshold = threshold;
}

static CBlock FindDevNetGenesisBlock(const CBlock &prevBlock, const CAmount& reward)
{
    std::string devNetName = gArgs.GetDevNetName();
    assert(!devNetName.empty());

    CBlock block = CreateDevNetGenesisBlock(prevBlock.GetHash(), devNetName.c_str(), prevBlock.nTime + 1, 0, prevBlock.nBits, reward);

    arith_uint256 bnTarget;
    bnTarget.SetCompact(block.nBits);

    for (uint32_t nNonce = 0; nNonce < UINT32_MAX; nNonce++) {
        block.nNonce = nNonce;

        uint256 hash = block.GetHash();
        if (UintToArith256(hash) <= bnTarget)
            return block;
    }

    // This is very unlikely to happen as we start the devnet with a very low difficulty. In many cases even the first
    // iteration of the above loop will give a result already
    error("FindDevNetGenesisBlock: could not find devnet genesis block for %s", devNetName);
    assert(false);
}

// this one is for testing only
static Consensus::LLMQParams llmq_test = {
        .type = Consensus::LLMQ_TEST,
        .name = "llmq_test",
        .size = 3,
        .minSize = 2,
        .threshold = 2,

        .dkgInterval = 30, // one DKG every 30 minutes
        .dkgPhaseBlocks = 3,
        .dkgMiningWindowStart = 15, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 27,
        .dkgBadVotesThreshold = 2,

        .signingActiveQuorumCount = 2, // just a few ones to allow easier testing

        .keepOldConnections = 3,
        .recoveryMembers = 3,
};

// this one is for testing only
static Consensus::LLMQParams llmq_test_v17 = {
        .type = Consensus::LLMQ_TEST_V17,
        .name = "llmq_test_v17",
        .size = 3,
        .minSize = 2,
        .threshold = 2,

        .dkgInterval = 30, // one DKG per 30 minutes
        .dkgPhaseBlocks = 3,
        .dkgMiningWindowStart = 15, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 27,
        .dkgBadVotesThreshold = 2,

        .signingActiveQuorumCount = 2, // just a few ones to allow easier testing

        .keepOldConnections = 3,
        .recoveryMembers = 3,
};

// this one is for devnets only
static Consensus::LLMQParams llmq_devnet = {
        .type = Consensus::LLMQ_DEVNET,
        .name = "llmq_devnet",
        .size = 10,
        .minSize = 7,
        .threshold = 6,

        .dkgInterval = 30, // one DKG per thirty minutes
        .dkgPhaseBlocks = 3,
        .dkgMiningWindowStart = 15, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 27,
        .dkgBadVotesThreshold = 7,

        .signingActiveQuorumCount = 3, // just a few ones to allow easier testing

        .keepOldConnections = 4,
        .recoveryMembers = 6,
};

static Consensus::LLMQParams llmq20_60 = {
        .type = Consensus::LLMQ_20_60,
        .name = "llmq_20_60",
        .size = 20,
        .minSize = 16,
        .threshold = 12,

        .dkgInterval = 60, // one DKG per hour
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 32,
        .dkgBadVotesThreshold = 14,

        .signingActiveQuorumCount = 24, // a full day worth of LLMQs

        .keepOldConnections = 25,
        .recoveryMembers = 12,
};

static Consensus::LLMQParams llmq40_60 = {
        .type = Consensus::LLMQ_40_60,
        .name = "llmq_40_60",
        .size = 40,
        .minSize = 30,
        .threshold = 24,

        .dkgInterval = 60 * 12, // one DKG every 12 hours
        .dkgPhaseBlocks = 6,
        .dkgMiningWindowStart = 30, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 42,
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // two days worth of LLMQs

        .keepOldConnections = 5,
        .recoveryMembers = 20,
};

// Used for deployment and min-proto-version signalling, so it needs a higher threshold
static Consensus::LLMQParams llmq40_85 = {
        .type = Consensus::LLMQ_40_85,
        .name = "llmq_40_85",
        .size = 40,
        .minSize = 35,
        .threshold = 34,

        .dkgInterval = 60 * 24, // one DKG every 24 hours
        .dkgPhaseBlocks = 6,
        .dkgMiningWindowStart = 30, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 60, // give it a larger mining window to make sure it is mined
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // four days worth of LLMQs

        .keepOldConnections = 5,
        .recoveryMembers = 20,
};

// Used for Platform
static Consensus::LLMQParams llmq20_70 = {
        .type = Consensus::LLMQ_20_70,
        .name = "llmq_20_70",
        .size = 20,
        .minSize = 16,
        .threshold = 14,

        .dkgInterval = 60, // one DKG per hour
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 32,
        .dkgBadVotesThreshold = 14,

        .signingActiveQuorumCount = 24, // a full day worth of LLMQs

        .keepOldConnections = 25,
        .recoveryMembers = 50,
};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params(bool useModulusV1) const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(consensus.zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);
    static CBigNum bnDecModulus = 0;
    if (!bnDecModulus)
        bnDecModulus.SetDec(consensus.zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsDec = libzerocoin::ZerocoinParams(bnDecModulus);

    if (useModulusV1)
        return &ZCParamsHex;

    return &ZCParamsDec;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210240; // Note: actual number of blocks per calendar year with DGW v3 is ~200700 (for example 449750 - 249050)
        consensus.nMasternodePaymentsStartBlock = 100000; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 158000; // actual historical value
        consensus.nMasternodePaymentsIncreasePeriod = 576*30; // 17280 - actual historical value
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 24;
        consensus.nBudgetPaymentsStartBlock = std::numeric_limits<int>::max();
        consensus.nBudgetPaymentsCycleBlocks = 43200; // (60*24*30)
        consensus.nBudgetPaymentsWindowBlocks = 2880;
        consensus.nSuperblockStartBlock = std::numeric_limits<int>::max();
        consensus.nSuperblockStartHash = uint256(); // do not check this
        consensus.nSuperblockCycle = 43200; // (60*24*30)
        consensus.nGovernanceMinQuorum = 10;
        consensus.nGovernanceFilterElements = 20000;
        consensus.nMasternodeMinimumConfirmations = 15;
        consensus.V17DeploymentHeight = 1669300;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("000002f68dbbf1fcfacb8f0b4e64083efdd2f07a906728ee068d573ffa5bcb4e");
        consensus.BIP65Height = consensus.V17DeploymentHeight;
        consensus.BIP66Height = 1; // 000002f68dbbf1fcfacb8f0b4e64083efdd2f07a906728ee068d573ffa5bcb4e
        consensus.CSVHeight = consensus.V17DeploymentHeight;
        consensus.BIP147Height = consensus.V17DeploymentHeight;
        consensus.DIP0001Height = consensus.V17DeploymentHeight;
        consensus.DIP0003Height = consensus.V17DeploymentHeight;
//        consensus.DIP0003EnforcementHeight = std::numeric_limits<int>::max();
        consensus.DIP0003EnforcementHash = uint256();
        consensus.DIP0008Height = consensus.V17DeploymentHeight;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Bytz: 1 day
        consensus.nPowTargetSpacing = 1 * 60; // Bytz: 1 minute
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;

        // Bytz specific parameters
        // Proof of Stake parameters
        consensus.nPosStartHeight = 201;
        consensus.nBlockTimeProtocolV2 = consensus.V17DeploymentHeight;
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.posLimit_V2 = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nTimeSlotLength = 15;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nPosTargetTimespan = 40 * 60; // 40 minutes
        consensus.nPosTargetTimespan_V2 = 2 * consensus.nTimeSlotLength * 60; // 30 minutes
        consensus.nStakeMinDepth = 600;
        consensus.nStakeMinAge = 60 * 60; // 1 hour
        consensus.nBlockStakeModifierV1A = 1000;
        consensus.nBlockStakeModifierV2 = consensus.V17DeploymentHeight;
        consensus.strCarbonOffsetAddress = "8GDeXyYNyc1o34v8BjtS3e1ZzvLDaqXNNK";
        // ATP parameters
        consensus.ATPStartHeight = consensus.V17DeploymentHeight;
        consensus.BytzAddrPrefix = "bytz";
        consensus.strTokenManagementKey = "sYCxBVHJx3A1tt7B1tFnaCJGnci3hvEf2c"; // 04d449cc1ac45d327c34d8b116797ad9ed287980a9199ea48dc4c8beab90ae2ded738e826ba0b27b5571d63884d985e2a50afbe8eef2925fc280af51a2a2d5e0e0
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // Other
        consensus.nCoinbaseMaturity = 100;
        consensus.AccruedCarbonOffsetStartHeight = consensus.V17DeploymentHeight;
        consensus.AccruedCarbonOffsetWindow = 1000;

        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = 25;
        consensus.nZerocoinStartTime = 1524496462;
        consensus.nBlockZerocoinV2 = 60;
        consensus.nPublicZCSpends = std::numeric_limits<int>::max();
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";

        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000e9b67326dfc16f4713f"); // 1623262

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xa3;
        pchMessageStart[1] = 0xea;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0x81;
        nDefaultPort = 37415;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1524496461, 67657104, 0x1e0ffff0, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000feb03167c4a4fa9f2bafcaea0e9f7e5646330e13c69e7ffa2dce58ace44"));
        assert(genesis.hashMerkleRoot == uint256S("0x80290404060ff7ff5bc6a42f755d24f6087ba5685474a5c8ffafac65de8b2bbf"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("main.seeder1.bytz.gg"); // Bytz US1 DNS Seeder
        vSeeds.emplace_back("main.seeder2.bytz.gg"); // Bytz EU1 DNS Seeder

        // Bytz addresses start with 'X'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,125);
        // Bytz script addresses start with '7'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,18);
        // Bytz private keys start with '7' or 'X'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,140);
        // Bytz BIP32 pubkeys start with 'xpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x02, 0x2D, 0x25, 0x33};
        // Bytz BIP32 prvkeys start with 'xprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x02, 0x21, 0x31, 0x2B};

        // Bytz BIP44 coin type is '416'
        nExtCoinType = 416;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_20_60] = llmq20_60;
        consensus.llmqs[Consensus::LLMQ_40_60] = llmq40_60;
        consensus.llmqs[Consensus::LLMQ_40_85] = llmq40_85;
        consensus.llmqs[Consensus::LLMQ_20_70] = llmq20_70;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_40_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_20_60;
        consensus.llmqTypePlatform = Consensus::LLMQ_20_70;

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;
        nLLMQConnectionRetryTimeout = 60;

        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 20;
        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour

        vSporkAddresses = {"sYJv3DxNMecQx7Z6FuQqqLGRBFjCVVpxmN"}; // 04a555983e950ddde95d4f236a1faa44eaf7399512342b40163afdc078dea8a941eb48834c61f22bd9632cbb2d7971bfdeddb6e476dc28726449eeb867affc5b26
        nMinSporkKeys = 1;
        fBIP9CheckMasternodesUpgraded = true;

        checkpointData = {
            {
                {0, uint256S("00000feb03167c4a4fa9f2bafcaea0e9f7e5646330e13c69e7ffa2dce58ace44")},     // Genesis block
                {1, uint256S("000002f68dbbf1fcfacb8f0b4e64083efdd2f07a906728ee068d573ffa5bcb4e")},     // First mined block
                {25, uint256S("0000016f6d9c834f269f07e624feb02ba725e3d954017549afde932c8f6d6dc7")},    // Zerocoin enabled
                {60, uint256S("00000039aca457e0dd2287e0fd636f1998e6b2774a64e8c18fa853776ec309c8")},    // Zerocoin V2 enabled
                {200, uint256S("000000078d815b257737d227d50e22f2486fd3ded21c5c0bca347a410c71bd26")},   // Switch PoW-PoS
                {201, uint256S("5542cf20a79e2658f45fc5385cc431035efae3980985254e01a08d930408bc52")},   // Switch PoW-PoS
                {202, uint256S("016023220b7e1578f923a126dddecbf345d8004734afb52636f60954ba116d21")},   // Switch PoW-PoS
                {300, uint256S("0cddd447eebbc7bd9f158bdc25eb1b290ab2b6f54ae77b07229c8da7b1999d99")},   //
                {700, uint256S("eac3327ace445de2f39a6209b3a778d370a7e6d676c254d82e3d1c8de272559b")},   // tx=1230 time=1526558980
                {67000, uint256S("727101d555687b91ed9740f3301048f3cfe5e5062babe491f2120ea7173b7234")}, // Add 3 premine blocks after this one
                {69713, uint256S("097a4a371b031eea8d26384a15e894dc60fcb7cd8304f62ab35c760317c36e28")}, // release v0.1.03
                {1623268, uint256S("1a0a8a556b6d95a44f7ba8c587879197051c1652e430ee3c3f57e5c173d80d38")}, // tx=3981853 time=1624868163
                {1669299, uint256S("039f15ac2d9a8e0adb7c9040428e8784782b20100185dc4949323c661617070a")}, // release v0.2.0.0
                {1669300, uint256S("fbfcaed9ada1df81ec70ba584576bd0128730d6e88866e93d4c4c512d82fb707")}, // release v0.2.0.0
                {1733710, uint256S("62257b9363e5e62207cb86539c93726ce6ba3901501d2ca992460d55193e82d7")}, // tx=4270019 time=1631277915
            }
        };

        chainTxData = ChainTxData{
            1631277915, // * UNIX timestamp of last known number of transactions (Block 1733710)
            4270019,    // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.044       // * estimated number of transactions per second after that timestamp
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nMasternodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 4030;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 4200;
        consensus.nBudgetPaymentsCycleBlocks = 144;
        consensus.nBudgetPaymentsWindowBlocks = 64;
        consensus.nSuperblockStartBlock = std::numeric_limits<int>::max(); // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on testnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on testnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.V17DeploymentHeight = 826130;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("0000065432f43b3efb23bd0f63fe33d00d02a5f36233fe1b982c08274d58ef12");
        consensus.BIP65Height = consensus.V17DeploymentHeight;
        consensus.BIP66Height = 1; // 0000065432f43b3efb23bd0f63fe33d00d02a5f36233fe1b982c08274d58ef12
        consensus.CSVHeight = consensus.V17DeploymentHeight;
        consensus.BIP147Height = consensus.V17DeploymentHeight;
        consensus.DIP0001Height = consensus.V17DeploymentHeight;
        consensus.DIP0003Height = consensus.V17DeploymentHeight;
//        consensus.DIP0003EnforcementHeight = std::numeric_limits<int>::max();
        consensus.DIP0003EnforcementHash = uint256();
        consensus.DIP0008Height = consensus.V17DeploymentHeight;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Bytz: 1 day
        consensus.nPowTargetSpacing = 1 * 60; // Bytz: 1 minute
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;

        // Bytz specific parameters
        // Proof of Stake parameters
        consensus.nPosStartHeight = 201;
        consensus.nBlockTimeProtocolV2 = consensus.V17DeploymentHeight;
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.posLimit_V2 = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nTimeSlotLength = 15;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nPosTargetTimespan = 40 * 60; // 40 minutes
        consensus.nPosTargetTimespan_V2 = 2 * consensus.nTimeSlotLength * 60; // 30 minutes
        consensus.nStakeMinDepth = 100;
        consensus.nStakeMinAge = 60 * 60; // 1 hour
        consensus.nBlockStakeModifierV1A = 51197;
        consensus.nBlockStakeModifierV2 = 826130;
        consensus.strCarbonOffsetAddress = "TqqiV3twXTaD5pL4vrA3nZqT8d8BPbxM3e";
        // ATP parameters
        consensus.ATPStartHeight = consensus.V17DeploymentHeight;
        consensus.BytzAddrPrefix = "bytztest";
        consensus.strTokenManagementKey = "TsdKwqnDKEN3N38QG5hTQBNJe6y1mdECy8";
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // Other
        consensus.nCoinbaseMaturity = 15;
        consensus.AccruedCarbonOffsetStartHeight = 831200;
        consensus.AccruedCarbonOffsetWindow = 100;
        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = 25;
        consensus.nZerocoinStartTime = 1524496462;
        consensus.nBlockZerocoinV2 = 60;
        consensus.nPublicZCSpends = std::numeric_limits<int>::max();
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";

        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000"); // 0

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0000009303aeadf8cf3812f5c869691dbd4cb118ad20e9bf553be434bafe6a52"); // 470000

        pchMessageStart[0] = 0x81;
        pchMessageStart[1] = 0xbb;
        pchMessageStart[2] = 0x9f;
        pchMessageStart[3] = 0x83;
        nDefaultPort = 47415;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1524496461, 846737, 0x1e0ffff0, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0000065432f43b3efb23bd0f63fe33d00d02a5f36233fe1b982c08274d58ef12"));
        //assert(genesis.hashMerkleRoot == uint256S("0xe0028eb9648db56b1ac77cf090b99048a8007e2bb64b68f092c03c7f56a662c7"));

        vFixedSeeds.clear();
        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet.seeder1.bytz.gg"); // Bytz US1 DNS Seeder
        vSeeds.emplace_back("testnet.seeder2.bytz.gg"); // Bytz EU1 DNS Seeder
        vSeeds.emplace_back("testnet.seeder3.bytz.gg"); // Bytz ASIA1 (Singapore) DNS Seeder
        vSeeds.emplace_back("testnet.seeder4.bytz.gg"); // Bytz AUSTRALIA1 (Sydney) DNS Seeder

        // Testnet Bytz addresses start with 'T'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,66);
        // Testnet Bytz script addresses start with '4' or '5'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,9);
        // Testnet private keys start with 'z' or '2'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,144);
        // Testnet Bytz BIP32 pubkeys start with 'DRKV' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x3A, 0x80, 0x61, 0xA0};
        // Testnet Bytz BIP32 prvkeys start with 'DRKP' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x3A, 0x80, 0x58, 0x37};

        // Testnet Bytz BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_TEST_V17] = llmq_test_v17;
        consensus.llmqs[Consensus::LLMQ_20_60] = llmq20_60;
        consensus.llmqs[Consensus::LLMQ_40_60] = llmq40_60;
        consensus.llmqs[Consensus::LLMQ_40_85] = llmq40_85;
        consensus.llmqs[Consensus::LLMQ_20_70] = llmq20_70;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_20_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_20_60;
        consensus.llmqTypePlatform = Consensus::LLMQ_20_70;

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = true;
        nLLMQConnectionRetryTimeout = 60;

        nPoolMinParticipants = 2;
        nPoolMaxParticipants = 20;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"TozWRrxnKYpshJw5PhAaP7gHzTLDFhKCnr"};
        nMinSporkKeys = 1;
        fBIP9CheckMasternodesUpgraded = true;

        checkpointData = {
            {
                {0, uint256S("0000065432f43b3efb23bd0f63fe33d00d02a5f36233fe1b982c08274d58ef12")},
            }
        };

        chainTxData = ChainTxData{
            1530893198, // * UNIX timestamp of last known number of transactions (Block 387900)
            4404,       // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.01        // * estimated number of transactions per second after that timestamp
        };

    }
};

/**
 * Devnet
 */
class CDevNetParams : public CChainParams {
public:
    CDevNetParams(bool fHelpOnly = false) {
        strNetworkID = "devnet";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nMasternodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 4030;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 4100;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 4200; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on devnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on devnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.V17DeploymentHeight = 1;
        consensus.BIP34Height = 1; // BIP34 activated immediately on devnet
        consensus.BIP65Height = 1; // BIP65 activated immediately on devnet
        consensus.BIP66Height = 1; // BIP66 activated immediately on devnet
        consensus.CSVHeight = consensus.V17DeploymentHeight;
        consensus.BIP147Height = consensus.V17DeploymentHeight;
        consensus.DIP0001Height = 2; // DIP0001 activated immediately on devnet
        consensus.DIP0003Height = 2; // DIP0003 activated immediately on devnet
//        consensus.DIP0003EnforcementHeight = 2; // DIP0003 activated immediately on devnet
        consensus.DIP0003EnforcementHash = uint256();
        consensus.DIP0008Height = 2; // DIP0008 activated immediately on devnet
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Bytz: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Bytz: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;

        // Bytz specific parameters
        // Proof of Stake parameters
        consensus.nPosStartHeight = 201;
        consensus.nBlockTimeProtocolV2 = consensus.V17DeploymentHeight;
        consensus.posLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.posLimit_V2 = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nTimeSlotLength = 15;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nPosTargetTimespan = 40 * 60; // 40 minutes
        consensus.nPosTargetTimespan_V2 = 2 * consensus.nTimeSlotLength * 60; // 30 minutes
        consensus.nStakeMinDepth = 100;
        consensus.nStakeMinAge = 60 * 60; // 1 hour
        consensus.nBlockStakeModifierV1A = 1000;
        consensus.nBlockStakeModifierV2 = consensus.V17DeploymentHeight;
        consensus.strCarbonOffsetAddress = "TkDutp66Ygp5PpPnrETvfyrtnxq5UevLpo";
        // ATP parameters
        consensus.ATPStartHeight = consensus.V17DeploymentHeight;
        consensus.AccruedCarbonOffsetStartHeight = consensus.V17DeploymentHeight;
        consensus.AccruedCarbonOffsetWindow = 100;
        consensus.BytzAddrPrefix = "bytztest";
        consensus.strTokenManagementKey = "TkDutp66Ygp5PpPnrETvfyrtnxq5UevLpo";
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // Other
        consensus.nCoinbaseMaturity = 15;
        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = std::numeric_limits<int>::max();
        consensus.nZerocoinStartTime = std::numeric_limits<int>::max();
        consensus.nBlockZerocoinV2 = std::numeric_limits<int>::max();
        consensus.nPublicZCSpends = std::numeric_limits<int>::max();
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";


        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        pchMessageStart[0] = 0xb2;
        pchMessageStart[1] = 0x8f;
        pchMessageStart[2] = 0xa3;
        pchMessageStart[3] = 0xcc;
        nDefaultPort = 47626;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1524496461, 12351, 0x207fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x618435c615f3d628acf97c19c4b3e6320555c62f515d4144425e4e8b7610fbab"));
//        assert(genesis.hashMerkleRoot == uint256S("0xe0028eb9648db56b1ac77cf090b99048a8007e2bb64b68f092c03c7f56a662c7"));

        if (!fHelpOnly) {
            devnetGenesis = FindDevNetGenesisBlock(genesis, 0 * COIN);
            consensus.hashDevnetGenesisBlock = devnetGenesis.GetHash();
        }

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("bytzevo.org",  "devnet-seed.bytzevo.org"));

        // Testnet Bytz addresses start with 'T'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,66);
        // Testnet Bytz script addresses start with '4' or '5'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,9);
        // Testnet private keys start with 'z' or '2'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,144);
        // Testnet Bytz BIP32 pubkeys start with 'DRKV' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x3A, 0x80, 0x61, 0xA0};
        // Testnet Bytz BIP32 prvkeys start with 'DRKP' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x3A, 0x80, 0x58, 0x37};

        // Testnet Bytz BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_DEVNET] = llmq_devnet;
        consensus.llmqs[Consensus::LLMQ_20_60] = llmq20_60;
        consensus.llmqs[Consensus::LLMQ_40_60] = llmq40_60;
        consensus.llmqs[Consensus::LLMQ_40_85] = llmq40_85;
        consensus.llmqs[Consensus::LLMQ_20_70] = llmq20_70;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_20_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_20_60;
        consensus.llmqTypePlatform = Consensus::LLMQ_20_70;

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;
        nLLMQConnectionRetryTimeout = 60;

        nPoolMinParticipants = 2;
        nPoolMaxParticipants = 20;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"TkDutp66Ygp5PpPnrETvfyrtnxq5Z1ub79"}; // 04b33722601343992c8a651fafa0f424c6ac90f797d3f58d90eebf96e817e9d7ca76a40e3c53b3d47f6f6a60b0d36dbb94ee630a5ad622f08d92782999fe7b043a
        nMinSporkKeys = 1;
        // devnets are started with no blocks and no MN, so we can't check for upgraded MN (as there are none)
        fBIP9CheckMasternodesUpgraded = false;

        checkpointData = (CCheckpointData) {
            {
                { 0, uint256S("0x000008ca1832a4baf228eb1553c03d3a2c8e02399550dd6ea8d65cec3ef23d2e")},
                { 1, devnetGenesis.GetHash() },
            }
        };

        chainTxData = ChainTxData{
            devnetGenesis.GetBlockTime(), // * UNIX timestamp of devnet genesis block
            2,                            // * we only have 2 coinbase transactions when a devnet is started up
            0.01                          // * estimated number of transactions per second
        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMasternodePaymentsStartBlock = 240;
        consensus.nMasternodePaymentsIncreaseBlock = 350;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 1000;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 1500;
        consensus.nSuperblockStartHash = uint256(); // do not check this on regtest
        consensus.nSuperblockCycle = 10;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 100;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.V17DeploymentHeight = 300;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = consensus.V17DeploymentHeight; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.CSVHeight = consensus.V17DeploymentHeight;
        consensus.BIP147Height = consensus.V17DeploymentHeight;
        consensus.DIP0001Height = 2000;
        consensus.DIP0003Height = 210;
//        consensus.DIP0003EnforcementHeight = 500;
        consensus.DIP0003EnforcementHash = uint256();
        consensus.DIP0008Height = 432;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Bytz: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Bytz: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;

        // Bytz specific parameters
        // Proof of Stake parameters
        consensus.nPosStartHeight = 201;
        consensus.nBlockTimeProtocolV2 = consensus.V17DeploymentHeight;
        consensus.posLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.posLimit_V2 = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nTimeSlotLength = 15;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nPosTargetTimespan = 40 * 60; // 40 minutes
        consensus.nPosTargetTimespan_V2 = 2 * consensus.nTimeSlotLength * 60; // 30 minutes
        consensus.nStakeMinDepth = 1;
        consensus.nStakeMinAge = 0;
        consensus.nBlockStakeModifierV1A = consensus.nPosStartHeight;
        consensus.nBlockStakeModifierV2 = consensus.V17DeploymentHeight;
        consensus.strCarbonOffsetAddress = "TqMgq4qkw7bGxf6CDhtDfEqzEtWD5C7x8U";
        // ATP parameters
        consensus.ATPStartHeight = consensus.V17DeploymentHeight;
        consensus.AccruedCarbonOffsetStartHeight = consensus.V17DeploymentHeight + 30;
        consensus.AccruedCarbonOffsetWindow = 10;
        consensus.BytzAddrPrefix = "bytzreg";
        consensus.strTokenManagementKey = "TqMgq4qkw7bGxf6CDhtDfEqzEtWD5C7x8U";
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // Other
        consensus.nCoinbaseMaturity = 15;
        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = std::numeric_limits<int>::max();
        consensus.nZerocoinStartTime = std::numeric_limits<int>::max();
        consensus.nBlockZerocoinV2 = std::numeric_limits<int>::max();
        consensus.nPublicZCSpends = std::numeric_limits<int>::max();
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";


        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xb2;
        pchMessageStart[1] = 0x8f;
        pchMessageStart[2] = 0xa3;
        pchMessageStart[3] = 0xcc;
        nDefaultPort = 47526;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1524496461, 12351, 0x207fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x618435c615f3d628acf97c19c4b3e6320555c62f515d4144425e4e8b7610fbab"));
//        assert(genesis.hashMerkleRoot == uint256S("0xe0028eb9648db56b1ac77cf090b99048a8007e2bb64b68f092c03c7f56a662c7"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fRequireRoutableExternalIP = false;
        fMineBlocksOnDemand = true;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;
        nLLMQConnectionRetryTimeout = 1; // must be lower then the LLMQ signing session timeout so that tests have control over failing behavior

        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes
        nPoolMinParticipants = 2;
        nPoolMaxParticipants = 20;

        // privKey: 5rE5LTDq3tRhaPW3RT1De35MocGc9wD8foaBGioxSXJsn45XaFG
        vSporkAddresses = {"TqMgq4qkw7bGxf6CDhtDfEqzEtWD5C7x8U"}; // 048b664010f7851071787d58c276c05701b7109fa29f2360a3e72b3bdfa32b49cf20a23fd34bcc49fc564fdbdccc54dd0dc9183a7bdf05d580d118fcdcd4abfb3f
        nMinSporkKeys = 1;
        // regtest usually has no masternodes in most tests, so don't check for upgraged MNs
        fBIP9CheckMasternodesUpgraded = false;

        checkpointData = {
            {
                {0, uint256S("0x000008ca1832a4baf228eb1553c03d3a2c8e02399550dd6ea8d65cec3ef23d2e")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        // Testnet Bytz addresses start with 'T'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,66);
        // Testnet Bytz script addresses start with '4' or '5'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,9);
        // Testnet private keys start with 'z' or '2'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,144);
        // Testnet Bytz BIP32 pubkeys start with 'DRKV' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x3A, 0x80, 0x61, 0xA0};
        // Testnet Bytz BIP32 prvkeys start with 'DRKP' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x3A, 0x80, 0x58, 0x37};

        // Regtest Bytz BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_TEST] = llmq_test;
        consensus.llmqs[Consensus::LLMQ_TEST_V17] = llmq_test_v17;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_TEST;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_TEST;
        consensus.llmqTypePlatform = Consensus::LLMQ_TEST;
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain, bool fHelpOnly)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::DEVNET) {
        return std::unique_ptr<CChainParams>(new CDevNetParams(fHelpOnly));
    } else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int64_t nWindowSize, int64_t nThresholdStart, int64_t nThresholdMin, int64_t nFalloffCoeff)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout, nWindowSize, nThresholdStart, nThresholdMin, nFalloffCoeff);
}

void UpdateDIP3Parameters(int nActivationHeight, int nEnforcementHeight)
{
    globalChainParams->UpdateDIP3Parameters(nActivationHeight, nEnforcementHeight);
}

void UpdateDIP8Parameters(int nActivationHeight)
{
    globalChainParams->UpdateDIP8Parameters(nActivationHeight);
}

void UpdateBudgetParameters(int nMasternodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
{
    globalChainParams->UpdateBudgetParameters(nMasternodePaymentsStartBlock, nBudgetPaymentsStartBlock, nSuperblockStartBlock);
}

void UpdateDevnetSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
{
    globalChainParams->UpdateSubsidyAndDiffParams(nMinimumDifficultyBlocks, nHighSubsidyBlocks, nHighSubsidyFactor);
}

void UpdateDevnetLLMQChainLocks(Consensus::LLMQType llmqType)
{
    globalChainParams->UpdateLLMQChainLocks(llmqType);
}

void UpdateDevnetLLMQInstantSend(Consensus::LLMQType llmqType)
{
    globalChainParams->UpdateLLMQInstantSend(llmqType);
}

void UpdateLLMQTestParams(int size, int threshold)
{
    globalChainParams->UpdateLLMQTestParams(size, threshold);
}

void UpdateLLMQDevnetParams(int size, int threshold)
{
    globalChainParams->UpdateLLMQDevnetParams(size, threshold);
}
