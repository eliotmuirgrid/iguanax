const HL7_2_5 =
{"nodes":{
 "ABS":{
  "desc":"Abstract",
  "type":"segment",
  "children":[
   {"type":"XCN", "desc":"Discharge Care Provider"      },
   {"type":"CE",  "desc":"Transfer Medical Service Code"},
   {"type":"CE",  "desc":"Severity Of Illness Code"     },
   {"type":"TS",  "desc":"Date/time Of Attestation"     },
   {"type":"XCN", "desc":"Attested By"                  },
   {"type":"CE",  "desc":"Triage Code"                  },
   {"type":"TS",  "desc":"Abstract Completion Date/time"},
   {"type":"XCN", "desc":"Abstracted By"                },
   {"type":"CE",  "desc":"Case Category Code"           },
   {"type":"ID",  "desc":"Caesarian Section Indicator"  },
   {"type":"CE",  "desc":"Gestation Category Code"      },
   {"type":"NM",  "desc":"Gestation Period - Weeks"     },
   {"type":"CE",  "desc":"Newborn Code"                 },
   {"type":"ID",  "desc":"Stillborn Indicator"          }]},
 "ACC":{
  "desc":"Accident",
  "type":"segment",
  "children":[
   {"type":"TS",  "desc":"Accident Date/time"            },
   {"type":"CE",  "desc":"Accident Code"                 },
   {"type":"ST",  "desc":"Accident Location"             },
   {"type":"CE",  "desc":"Auto Accident State"           },
   {"type":"ID",  "desc":"Accident Job Related Indicator"},
   {"type":"ID",  "desc":"Accident Death Indicator"      },
   {"type":"XCN", "desc":"Entered By"                    },
   {"type":"ST",  "desc":"Accident Description"          },
   {"type":"ST",  "desc":"Brought In By"                 },
   {"type":"ID",  "desc":"Police Notified Indicator"     },
   {"type":"XAD", "desc":"Accident Address"              }]},
 "ACK":{
  "desc":"Acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"         ,"req":true},
   {"type":"SFT", "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR", "desc":"Error"                  ,"repeats":true}]},
 "AD":{
  "desc":"Address",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Street Address"              },
   {"type":"ST", "desc":"Other Designation"           },
   {"type":"ST", "desc":"City"                        },
   {"type":"ST", "desc":"State Or Province"           },
   {"type":"ST", "desc":"Zip Or Postal Code"          },
   {"type":"ID", "desc":"Country"                     },
   {"type":"ID", "desc":"Address Type"                },
   {"type":"ST", "desc":"Other Geographic Designation"}]},
 "ADD":{
  "desc":"Addendum",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Addendum Continuation Pointer"}]},
 "ADMINISTRATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA",                        "desc":"Pharmacy/Treatment Administration" ,"req":true ,"repeats":true},
   {"type":"RXR",                        "desc":"Pharmacy/Treatment Route"          ,"req":true},
   {"type":"OBSERVATION_ADMINISTRATION", "desc":""                                  ,"repeats":true}]},
 "ADMINISTRATION_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration"  ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ADMINISTRATION_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true ,"repeats":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"          ,"req":true}]},
 "ADRA19":{
  "desc":"Patient Query - Response",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"              ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",            "desc":"Message Acknowledgment"      ,"req":true},
   {"type":"ERR",            "desc":"Error"                      },
   {"type":"QAK",            "desc":"Query Acknowledgment"       },
   {"type":"QRD",            "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",            "desc":"Original Style Query Filter"},
   {"type":"QUERY_RESPONSE", "desc":""                            ,"req":true ,"repeats":true},
   {"type":"DSC",            "desc":"Continuation Pointer"       }]},
 "ADTA01":{
  "desc":"Admit/visit notification",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA01", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA01", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       },
   {"type":"PDA",              "desc":"Patient Death And Autopsy"       }]},
 "ADTA02":{
  "desc":"Transfer a patient",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"ROL", "desc":"Role"                           ,"repeats":true},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"ROL", "desc":"Role"                           ,"repeats":true},
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"PDA", "desc":"Patient Death And Autopsy"     }]},
 "ADTA03":{
  "desc":"Discharge/end visit",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA03", "desc":""                                 ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA03", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"PDA",              "desc":"Patient Death And Autopsy"       }]},
 "ADTA04":{
  "desc":"Register a patient",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA04", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA04", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       },
   {"type":"PDA",              "desc":"Patient Death And Autopsy"       }]},
 "ADTA05":{
  "desc":"Pre-admit a patient",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA05", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA05", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       }]},
 "ADTA06":{
  "desc":"Change an outpatient to an inpatient",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"MRG",              "desc":"Merge Patient Information"       },
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA06", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA06", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       }]},
 "ADTA07":{
  "desc":"Change an inpatient to an outpatient",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"MRG",              "desc":"Merge Patient Information"       },
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA07", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA07", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       }]},
 "ADTA08":{
  "desc":"Update patient information",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA08", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA08", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       },
   {"type":"PDA",              "desc":"Patient Death And Autopsy"       }]},
 "ADTA09":{
  "desc":"Patient departing - tracking",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "ADTA10":{
  "desc":"Patient arriving - tracking",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "ADTA11":{
  "desc":"Cancel admit/visit notification",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "ADTA12":{
  "desc":"Cancel transfer",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                     }]},
 "ADTA13":{
  "desc":"Cancel discharge/end visit",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA13", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA13", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       },
   {"type":"PDA",              "desc":"Patient Death And Autopsy"       }]},
 "ADTA14":{
  "desc":"Pending admit",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA14", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA14", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       }]},
 "ADTA15":{
  "desc":"Pending transfer",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"ROL", "desc":"Role"                           ,"repeats":true},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"ROL", "desc":"Role"                           ,"repeats":true},
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "ADTA16":{
  "desc":"Pending discharge",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA16", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA16", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        }]},
 "ADTA17":{
  "desc":"Swap patients",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA18":{
  "desc":"Merge patient information (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true}]},
 "ADTA20":{
  "desc":"Bed status update",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"    ,"req":true},
   {"type":"SFT", "desc":"Software Segment"  ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"        ,"req":true},
   {"type":"NPU", "desc":"Bed Status Update" ,"req":true}]},
 "ADTA21":{
  "desc":"Patient goes on a \"leave of absence\"",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA22":{
  "desc":"Patient returns from a \"leave of absence\"",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA23":{
  "desc":"Delete a patient record",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA24":{
  "desc":"Link patient information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true}]},
 "ADTA25":{
  "desc":"Cancel pending discharge",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA26":{
  "desc":"Cancel pending transfer",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA27":{
  "desc":"Cancel pending admit",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA28":{
  "desc":"Add person information",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA28", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA28", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       }]},
 "ADTA29":{
  "desc":"Delete person information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA30":{
  "desc":"Merge person information (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA31":{
  "desc":"Update person information",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                       ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",              "desc":"Additional Information"          },
   {"type":"ROL",              "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",              "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_ADTA31", "desc":""                                 ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_ADTA31", "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"UB1",              "desc":"UB82"                            },
   {"type":"UB2",              "desc":"UB92 Data"                       }]},
 "ADTA32":{
  "desc":"Cancel patient arriving - tracking",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA33":{
  "desc":"Cancel patient departing - tracking",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true}]},
 "ADTA34":{
  "desc":"Merge patient information - patient ID only (for backward compatibility)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA35":{
  "desc":"Merge patient information - account number only (for backward compatibility)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA36":{
  "desc":"Merge patient information - patient ID and account number (for backward compatibility)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA37":{
  "desc":"Unlink patient information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true}]},
 "ADTA38":{
  "desc":"Cancel pre-admit",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true},
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true},
   {"type":"DRG", "desc":"Diagnosis Related Group"       }]},
 "ADTA39":{
  "desc":"Merge person - patient ID (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH",     "desc":"Message Header"   ,"req":true},
   {"type":"SFT",     "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",     "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT", "desc":""                 ,"req":true ,"repeats":true}]},
 "ADTA40":{
  "desc":"Merge patient - patient identifier list",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",            "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT_ADTA40", "desc":""                 ,"req":true ,"repeats":true}]},
 "ADTA41":{
  "desc":"Merge account - patient account number",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",            "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT_ADTA41", "desc":""                 ,"req":true ,"repeats":true}]},
 "ADTA42":{
  "desc":"Merge visit - visit number",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",            "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT_ADTA42", "desc":""                 ,"req":true ,"repeats":true}]},
 "ADTA43":{
  "desc":"Move patient information - patient identifier list",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",            "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT_ADTA43", "desc":""                 ,"req":true ,"repeats":true}]},
 "ADTA44":{
  "desc":"Move account information - patient account number",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",            "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT_ADTA44", "desc":""                 ,"req":true ,"repeats":true}]},
 "ADTA45":{
  "desc":"Move visit information - visit number",
  "type":"message",
  "children":[
   {"type":"MSH",        "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",        "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN",        "desc":"Event Type"                     ,"req":true},
   {"type":"PID",        "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",        "desc":"Patient Additional Demographic"},
   {"type":"MERGE_INFO", "desc":""                               ,"req":true ,"repeats":true}]},
 "ADTA46":{
  "desc":"Change patient ID (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA47":{
  "desc":"Change patient identifier list",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA48":{
  "desc":"Change alternate patient ID (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA49":{
  "desc":"Change patient account number",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "ADTA50":{
  "desc":"Change visit number",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true}]},
 "ADTA51":{
  "desc":"Change alternate visit ID",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true}]},
 "ADTA52":{
  "desc":"Cancel leave of absence for a patient",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        }]},
 "ADTA53":{
  "desc":"Cancel patient returns from a leave of absence",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        }]},
 "ADTA54":{
  "desc":"Change attending doctor",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"ROL", "desc":"Role"                           ,"repeats":true},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"ROL", "desc":"Role"                           ,"repeats":true}]},
 "ADTA55":{
  "desc":"Cancel change attending doctor",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"PV2", "desc":"Additional Information"        }]},
 "ADTA60":{
  "desc":"Update allergy information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                       ,"req":true},
   {"type":"SFT", "desc":"Software Segment"                     ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                           ,"req":true},
   {"type":"PID", "desc":"Patient Identification"               ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                       },
   {"type":"PV2", "desc":"Additional Information"              },
   {"type":"IAM", "desc":"Patient Adverse Reaction Information" ,"repeats":true}]},
 "ADTA61":{
  "desc":"Change consulting doctor",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"ROL", "desc":"Role"                           ,"repeats":true},
   {"type":"PV2", "desc":"Additional Information"        }]},
 "ADTA62":{
  "desc":"Cancel change consulting doctor",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                  ,"req":true},
   {"type":"ROL", "desc":"Role"                           ,"repeats":true},
   {"type":"PV2", "desc":"Additional Information"        }]},
 "AFF":{
  "desc":"Professional Affiliation",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID _ Aff"                                     ,"req":true},
   {"type":"XON", "desc":"Professional Organization"                        ,"req":true},
   {"type":"XAD", "desc":"Professional Organization Address"               },
   {"type":"DR",  "desc":"Professional Organization Affiliation Date Range" ,"repeats":true},
   {"type":"ST",  "desc":"Professional Affiliation Additional Information" }]},
 "AIG":{
  "desc":"Appointment Information - General Resource",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Aig"                 ,"req":true},
   {"type":"ID", "desc":"Segment Action Code"         },
   {"type":"CE", "desc":"Resource ID"                 },
   {"type":"CE", "desc":"Resource Type"                ,"req":true},
   {"type":"CE", "desc":"Resource Group"               ,"repeats":true},
   {"type":"NM", "desc":"Resource Quantity"           },
   {"type":"CE", "desc":"Resource Quantity Units"     },
   {"type":"TS", "desc":"Start Date/time"             },
   {"type":"NM", "desc":"Start Date/time Offset"      },
   {"type":"CE", "desc":"Start Date/time Offset Units"},
   {"type":"NM", "desc":"Duration"                    },
   {"type":"CE", "desc":"Duration Units"              },
   {"type":"IS", "desc":"Allow Substitution Code"     },
   {"type":"CE", "desc":"Filler Status Code"          }]},
 "AIL":{
  "desc":"Appointment Information - Location Resource",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Ail"                 ,"req":true},
   {"type":"ID", "desc":"Segment Action Code"         },
   {"type":"PL", "desc":"Location Resource ID"         ,"repeats":true},
   {"type":"CE", "desc":"Location Type-ail"           },
   {"type":"CE", "desc":"Location Group"              },
   {"type":"TS", "desc":"Start Date/time"             },
   {"type":"NM", "desc":"Start Date/time Offset"      },
   {"type":"CE", "desc":"Start Date/time Offset Units"},
   {"type":"NM", "desc":"Duration"                    },
   {"type":"CE", "desc":"Duration Units"              },
   {"type":"IS", "desc":"Allow Substitution Code"     },
   {"type":"CE", "desc":"Filler Status Code"          }]},
 "AIP":{
  "desc":"Appointment Information - Personnel Resource",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Aip"                 ,"req":true},
   {"type":"ID",  "desc":"Segment Action Code"         },
   {"type":"XCN", "desc":"Personnel Resource ID"        ,"repeats":true},
   {"type":"CE",  "desc":"Resource Type"               },
   {"type":"CE",  "desc":"Resource Group"              },
   {"type":"TS",  "desc":"Start Date/time"             },
   {"type":"NM",  "desc":"Start Date/time Offset"      },
   {"type":"CE",  "desc":"Start Date/time Offset Units"},
   {"type":"NM",  "desc":"Duration"                    },
   {"type":"CE",  "desc":"Duration Units"              },
   {"type":"IS",  "desc":"Allow Substitution Code"     },
   {"type":"CE",  "desc":"Filler Status Code"          }]},
 "AIS":{
  "desc":"Appointment Information",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Ais"                            ,"req":true},
   {"type":"ID", "desc":"Segment Action Code"                    },
   {"type":"CE", "desc":"Universal Service Identifier"            ,"req":true},
   {"type":"TS", "desc":"Start Date/time"                        },
   {"type":"NM", "desc":"Start Date/time Offset"                 },
   {"type":"CE", "desc":"Start Date/time Offset Units"           },
   {"type":"NM", "desc":"Duration"                               },
   {"type":"CE", "desc":"Duration Units"                         },
   {"type":"IS", "desc":"Allow Substitution Code"                },
   {"type":"CE", "desc":"Filler Status Code"                     },
   {"type":"CE", "desc":"Placer Supplemental Service Information" ,"repeats":true},
   {"type":"CE", "desc":"Filler Supplemental Service Information" ,"repeats":true}]},
 "AL1":{
  "desc":"Patient Allergy Information",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Al1"                       ,"req":true},
   {"type":"CE", "desc":"Allergen Type Code"                },
   {"type":"CE", "desc":"Allergen Code/mnemonic/description" ,"req":true},
   {"type":"CE", "desc":"Allergy Severity Code"             },
   {"type":"ST", "desc":"Allergy Reaction Code"              ,"repeats":true},
   {"type":"DT", "desc":"Identification Date"               }]},
 "ALLERGY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AL1",           "desc":"Patient Allergy Information" ,"req":true ,"repeats":true},
   {"type":"VISIT_ALLERGY", "desc":""                           }]},
 "APP_STATS":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NST", "desc":"Application Control Level Statistics" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                   ,"repeats":true}]},
 "APP_STATUS":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NSC", "desc":"Application Status Change" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"        ,"repeats":true}]},
 "APR":{
  "desc":"Appointment Preferences",
  "type":"segment",
  "children":[
   {"type":"SCV", "desc":"Time Selection Criteria"     ,"repeats":true},
   {"type":"SCV", "desc":"Resource Selection Criteria" ,"repeats":true},
   {"type":"SCV", "desc":"Location Selection Criteria" ,"repeats":true},
   {"type":"NM",  "desc":"Slot Spacing Criteria"      },
   {"type":"SCV", "desc":"Filler Override Criteria"    ,"repeats":true}]},
 "ARQ":{
  "desc":"Appointment Request",
  "type":"segment",
  "children":[
   {"type":"EI",  "desc":"Placer Appointment ID"           ,"req":true},
   {"type":"EI",  "desc":"Filler Appointment ID"          },
   {"type":"NM",  "desc":"Occurrence Number"              },
   {"type":"EI",  "desc":"Placer Group Number"            },
   {"type":"CE",  "desc":"Schedule ID"                    },
   {"type":"CE",  "desc":"Request Event Reason"           },
   {"type":"CE",  "desc":"Appointment Reason"             },
   {"type":"CE",  "desc":"Appointment Type"               },
   {"type":"NM",  "desc":"Appointment Duration"           },
   {"type":"CE",  "desc":"Appointment Duration Units"     },
   {"type":"DR",  "desc":"Requested Start Date/time Range" ,"repeats":true},
   {"type":"ST",  "desc":"Priority-arq"                   },
   {"type":"RI",  "desc":"Repeating Interval"             },
   {"type":"ST",  "desc":"Repeating Interval Duration"    },
   {"type":"XCN", "desc":"Placer Contact Person"           ,"req":true ,"repeats":true},
   {"type":"XTN", "desc":"Placer Contact Phone Number"     ,"repeats":true},
   {"type":"XAD", "desc":"Placer Contact Address"          ,"repeats":true},
   {"type":"PL",  "desc":"Placer Contact Location"        },
   {"type":"XCN", "desc":"Entered By Person"               ,"req":true ,"repeats":true},
   {"type":"XTN", "desc":"Entered By Phone Number"         ,"repeats":true},
   {"type":"PL",  "desc":"Entered By Location"            },
   {"type":"EI",  "desc":"Parent Placer Appointment ID"   },
   {"type":"EI",  "desc":"Parent Filler Appointment ID"   },
   {"type":"EI",  "desc":"Placer Order Number"             ,"repeats":true},
   {"type":"EI",  "desc":"Filler Order Number"             ,"repeats":true}]},
 "ASSOCIATED_PERSON":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NK1",                 "desc":"Next Of Kin / Associated Parties" ,"req":true},
   {"type":"ASSOCIATED_RX_ORDER", "desc":""                                },
   {"type":"ASSOCIATED_RX_ADMIN", "desc":""                                 ,"repeats":true},
   {"type":"PRB",                 "desc":"Problem Details"                  ,"repeats":true},
   {"type":"OBX",                 "desc":"Observation/Result"               ,"repeats":true}]},
 "ASSOCIATED_PERSON_PEX_CAUSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NK1",                                   "desc":"Next Of Kin / Associated Parties" ,"req":true},
   {"type":"ASSOCIATED_RX_ORDER_ASSOCIATED_PERSON", "desc":""                                },
   {"type":"ASSOCIATED_RX_ADMIN_ASSOCIATED_PERSON", "desc":""                                 ,"repeats":true},
   {"type":"PRB",                                   "desc":"Problem Details"                  ,"repeats":true},
   {"type":"OBX",                                   "desc":"Observation/Result"               ,"repeats":true}]},
 "ASSOCIATED_RX_ADMIN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"         }]},
 "ASSOCIATED_RX_ADMIN_ASSOCIATED_PERSON":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"         }]},
 "ASSOCIATED_RX_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",            "desc":"Pharmacy/Treatment Encoded Order" ,"req":true},
   {"type":"NK1_TIMING_QTY", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"RXR",            "desc":"Pharmacy/Treatment Route"         ,"repeats":true}]},
 "ASSOCIATED_RX_ORDER_ASSOCIATED_PERSON":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                                "desc":"Pharmacy/Treatment Encoded Order" ,"req":true},
   {"type":"NK1_TIMING_QTY_ASSOCIATED_RX_ORDER", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"RXR",                                "desc":"Pharmacy/Treatment Route"         ,"repeats":true}]},
 "AUI":{
  "desc":"Authorization Information",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Authorization Number"},
   {"type":"DT", "desc":"Date"                },
   {"type":"ST", "desc":"Source"              }]},
 "AUT":{
  "desc":"Authorization Information",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Authorizing Payor, Plan ID"     },
   {"type":"CE", "desc":"Authorizing Payor, Company ID"   ,"req":true},
   {"type":"ST", "desc":"Authorizing Payor, Company Name"},
   {"type":"TS", "desc":"Authorization Effective Date"   },
   {"type":"TS", "desc":"Authorization Expiration Date"  },
   {"type":"EI", "desc":"Authorization Identifier"       },
   {"type":"CP", "desc":"Reimbursement Limit"            },
   {"type":"NM", "desc":"Requested Number Of Treatments" },
   {"type":"NM", "desc":"Authorized Number Of Treatments"},
   {"type":"TS", "desc":"Process Date"                   }]},
 "AUTHORIZATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_1_RPAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_1_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_1_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_2_PROCEDURE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_2_PROCEDURE_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_2_PROCEDURE_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group3_RRII13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group3_RRII14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group3_RRII15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group4":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group4_REFI13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group4_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_Group4_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_REFI13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_RRII12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_RRII13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_RRII14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_CONTACT_RRII15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_Group5":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_Group5_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_Group5_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_Group5_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "AUTHORIZATION_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AUT", "desc":"Authorization Information" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"             }]},
 "BARP01":{
  "desc":"Add patient accounts",
  "type":"message",
  "children":[
   {"type":"MSH",   "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",   "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN",   "desc":"Event Type"                     ,"req":true},
   {"type":"PID",   "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",   "desc":"Patient Additional Demographic"},
   {"type":"ROL",   "desc":"Role"                           ,"repeats":true},
   {"type":"VISIT", "desc":""                               ,"req":true ,"repeats":true}]},
 "BARP02":{
  "desc":"Purge patient accounts",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",            "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT_BARP02", "desc":""                 ,"req":true ,"repeats":true}]},
 "BARP05":{
  "desc":"Update account",
  "type":"message",
  "children":[
   {"type":"MSH",          "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",          "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN",          "desc":"Event Type"                     ,"req":true},
   {"type":"PID",          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",          "desc":"Patient Additional Demographic"},
   {"type":"ROL",          "desc":"Role"                           ,"repeats":true},
   {"type":"VISIT_BARP05", "desc":""                               ,"req":true ,"repeats":true}]},
 "BARP06":{
  "desc":"End account",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"EVN",            "desc":"Event Type"       ,"req":true},
   {"type":"PATIENT_BARP06", "desc":""                 ,"req":true ,"repeats":true}]},
 "BARP10":{
  "desc":"Transmit Ambulatory Payment  Classification(APC)",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"         ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"       ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"             ,"req":true},
   {"type":"PID",              "desc":"Patient Identification" ,"req":true},
   {"type":"PV1",              "desc":"Patient Visit"          ,"req":true},
   {"type":"DG1",              "desc":"Diagnosis"              ,"repeats":true},
   {"type":"GP1",              "desc":"Visit"                  ,"req":true},
   {"type":"PROCEDURE_BARP10", "desc":""                       ,"repeats":true}]},
 "BARP12":{
  "desc":"Update Diagnosis/Procedure",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"        ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"              ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"  ,"req":true},
   {"type":"PV1",              "desc":"Patient Visit"           ,"req":true},
   {"type":"DG1",              "desc":"Diagnosis"               ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"},
   {"type":"PROCEDURE_BARP12", "desc":""                        ,"repeats":true}]},
 "BHS":{
  "desc":"Batch Header",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Batch Field Separator"       ,"req":true},
   {"type":"ST", "desc":"Batch Encoding Characters"   ,"req":true},
   {"type":"HD", "desc":"Batch Sending Application"  },
   {"type":"HD", "desc":"Batch Sending Facility"     },
   {"type":"HD", "desc":"Batch Receiving Application"},
   {"type":"HD", "desc":"Batch Receiving Facility"   },
   {"type":"TS", "desc":"Batch Creation Date/time"   },
   {"type":"ST", "desc":"Batch Security"             },
   {"type":"ST", "desc":"Batch Name/id/type"         },
   {"type":"ST", "desc":"Batch Comment"              },
   {"type":"ST", "desc":"Batch Control ID"           },
   {"type":"ST", "desc":"Reference Batch Control ID" }]},
 "BLC":{
  "desc":"Blood Code",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Blood Product Code"},
   {"type":"CQ", "desc":"Blood Amount"      }]},
 "BLG":{
  "desc":"Billing",
  "type":"segment",
  "children":[
   {"type":"CCD", "desc":"When To Charge"    },
   {"type":"ID",  "desc":"Charge Type"       },
   {"type":"CX",  "desc":"Account ID"        },
   {"type":"CWE", "desc":"Charge Type Reason"}]},
 "BPO":{
  "desc":"Blood Product Order",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID _ Bpo"                       ,"req":true},
   {"type":"CWE", "desc":"Bp Universal Service ID"            ,"req":true},
   {"type":"CWE", "desc":"Bp Processing Requirements"         ,"repeats":true},
   {"type":"NM",  "desc":"Bp Quantity"                        ,"req":true},
   {"type":"NM",  "desc":"Bp Amount"                         },
   {"type":"CE",  "desc":"Bp Units"                          },
   {"type":"TS",  "desc":"Bp Intended Use Date/time"         },
   {"type":"PL",  "desc":"Bp Intended Dispense From Location"},
   {"type":"XAD", "desc":"Bp Intended Dispense From Address" },
   {"type":"TS",  "desc":"Bp Requested Dispense Date/time"   },
   {"type":"PL",  "desc":"Bp Requested Dispense To Location" },
   {"type":"XAD", "desc":"Bp Requested Dispense To Address"  },
   {"type":"CWE", "desc":"Bp Indication For Use"              ,"repeats":true},
   {"type":"ID",  "desc":"Bp Informed Consent Indicator"     }]},
 "BPSO29":{
  "desc":"Blood product dispense status message",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_BPSO29", "desc":""                  },
   {"type":"ORDER",          "desc":""                   ,"req":true ,"repeats":true}]},
 "BPX":{
  "desc":"Blood Product Dispense Status",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID _ Bpx"                    ,"req":true},
   {"type":"CWE", "desc":"Bp Dispense Status"              ,"req":true},
   {"type":"ID",  "desc":"Bp Status"                       ,"req":true},
   {"type":"TS",  "desc":"Bp Date/time Of Status"          ,"req":true},
   {"type":"EI",  "desc":"Bc Donation ID"                 },
   {"type":"CNE", "desc":"Bc Component"                   },
   {"type":"CNE", "desc":"Bc Donation Type / Intended Use"},
   {"type":"CWE", "desc":"Cp Commercial Product"          },
   {"type":"XON", "desc":"Cp Manufacturer"                },
   {"type":"EI",  "desc":"Cp Lot Number"                  },
   {"type":"CNE", "desc":"Bp Blood Group"                 },
   {"type":"CNE", "desc":"Bc Special Testing"              ,"repeats":true},
   {"type":"TS",  "desc":"Bp Expiration Date/time"        },
   {"type":"NM",  "desc":"Bp Quantity"                     ,"req":true},
   {"type":"NM",  "desc":"Bp Amount"                      },
   {"type":"CE",  "desc":"Bp Units"                       },
   {"type":"EI",  "desc":"Bp Unique ID"                   },
   {"type":"PL",  "desc":"Bp Actual Dispensed To Location"},
   {"type":"XAD", "desc":"Bp Actual Dispensed To Address" },
   {"type":"XCN", "desc":"Bp Dispensed To Receiver"       },
   {"type":"XCN", "desc":"Bp Dispensing Individual"       }]},
 "BRPO30":{
  "desc":"Blood product dispense status acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"PATIENT_BRPO30", "desc":""                       ,"repeats":true}]},
 "BRTO32":{
  "desc":"Blood product transfusion/disposition acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",      "desc":"Message Header"         ,"req":true},
   {"type":"MSA",      "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",      "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",      "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",      "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE", "desc":""                      }]},
 "BTS":{
  "desc":"Batch Trailer",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Batch Message Count"},
   {"type":"ST", "desc":"Batch Comment"      },
   {"type":"NM", "desc":"Batch Totals"        ,"repeats":true}]},
 "BTSO31":{
  "desc":"Blood product transfusion/disposition message",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_BTSO31", "desc":""                  },
   {"type":"ORDER_BTSO31",   "desc":""                   ,"req":true ,"repeats":true}]},
 "BTX":{
  "desc":"Blood Product Transfusion/Disposition",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID _ Btx"                             ,"req":true},
   {"type":"EI",  "desc":"Bc Donation ID"                          },
   {"type":"CNE", "desc":"Bc Component"                            },
   {"type":"CNE", "desc":"Bc Blood Group"                          },
   {"type":"CWE", "desc":"Cp Commercial Product"                   },
   {"type":"XON", "desc":"Cp Manufacturer"                         },
   {"type":"EI",  "desc":"Cp Lot Number"                           },
   {"type":"NM",  "desc":"Bp Quantity"                              ,"req":true},
   {"type":"NM",  "desc":"Bp Amount"                               },
   {"type":"CE",  "desc":"Bp Units"                                },
   {"type":"CWE", "desc":"Bp Transfusion/disposition Status"        ,"req":true},
   {"type":"ID",  "desc":"Bp Message Status"                        ,"req":true},
   {"type":"TS",  "desc":"Bp Date/time Of Status"                   ,"req":true},
   {"type":"XCN", "desc":"Bp Administrator"                        },
   {"type":"XCN", "desc":"Bp Verifier"                             },
   {"type":"TS",  "desc":"Bp Transfusion Start Date/time Of Status"},
   {"type":"TS",  "desc":"Bp Transfusion End Date/time Of Status"  },
   {"type":"CWE", "desc":"Bp Adverse Reaction Type"                 ,"repeats":true},
   {"type":"CWE", "desc":"Bp Transfusion Interrupted Reason"       }]},
 "CCD":{
  "desc":"Charge Code And Date",
  "type":"composite",
  "children":[
   {"type":"ID", "desc":"Invocation Event"},
   {"type":"TS", "desc":"Date/time"       }]},
 "CDM":{
  "desc":"Charge Description Master",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Primary Key Value - Cdm"        ,"req":true},
   {"type":"CE",  "desc":"Charge Code Alias"              ,"repeats":true},
   {"type":"ST",  "desc":"Charge Description Short"       ,"req":true},
   {"type":"ST",  "desc":"Charge Description Long"       },
   {"type":"IS",  "desc":"Description Override Indicator"},
   {"type":"CE",  "desc":"Exploding Charges"              ,"repeats":true},
   {"type":"CE",  "desc":"Procedure Code"                 ,"repeats":true},
   {"type":"ID",  "desc":"Active/inactive Flag"          },
   {"type":"CE",  "desc":"Inventory Number"               ,"repeats":true},
   {"type":"NM",  "desc":"Resource Load"                 },
   {"type":"CX",  "desc":"Contract Number"                ,"repeats":true},
   {"type":"XON", "desc":"Contract Organization"          ,"repeats":true},
   {"type":"ID",  "desc":"Room Fee Indicator"            }]},
 "CE":{
  "desc":"Coded Element",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Identifier"                     },
   {"type":"ST", "desc":"Text"                           },
   {"type":"ID", "desc":"Name Of Coding System"          },
   {"type":"ST", "desc":"Alternate Identifier"           },
   {"type":"ST", "desc":"Alternate Text"                 },
   {"type":"ID", "desc":"Name Of Alternate Coding System"}]},
 "CER":{
  "desc":"Certificate Detail",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID _ Cer"                                                     ,"req":true},
   {"type":"ST",  "desc":"Serial Number"                                                   },
   {"type":"ST",  "desc":"Version"                                                         },
   {"type":"XON", "desc":"Granting Authority"                                              },
   {"type":"XCN", "desc":"Issuing Authority"                                               },
   {"type":"ED",  "desc":"Signature Of Issuing Authority"                                  },
   {"type":"ID",  "desc":"Granting Country"                                                },
   {"type":"CWE", "desc":"Granting State/province"                                         },
   {"type":"CWE", "desc":"Granting County/parish"                                          },
   {"type":"CWE", "desc":"Certificate Type"                                                },
   {"type":"CWE", "desc":"Certificate Domain"                                              },
   {"type":"ID",  "desc":"Subject ID"                                                      },
   {"type":"ST",  "desc":"Subject Name"                                                     ,"req":true},
   {"type":"CWE", "desc":"Subject Directory Attribute Extension (health Professional Data)" ,"repeats":true},
   {"type":"CWE", "desc":"Subject Public Key Info"                                         },
   {"type":"CWE", "desc":"Authority Key Identifier"                                        },
   {"type":"ID",  "desc":"Basic Constraint"                                                },
   {"type":"CWE", "desc":"Crl Distribution Point"                                           ,"repeats":true},
   {"type":"ID",  "desc":"Jurisdiction Country"                                            },
   {"type":"CWE", "desc":"Jurisdiction State/province"                                     },
   {"type":"CWE", "desc":"Jurisdiction County/parish"                                      },
   {"type":"CWE", "desc":"Jurisdiction Breadth"                                             ,"repeats":true},
   {"type":"TS",  "desc":"Granting Date"                                                   },
   {"type":"TS",  "desc":"Issuing Date"                                                    },
   {"type":"TS",  "desc":"Activation Date"                                                 },
   {"type":"TS",  "desc":"Inactivation Date"                                               },
   {"type":"TS",  "desc":"Expiration Date"                                                 },
   {"type":"TS",  "desc":"Renewal Date"                                                    },
   {"type":"TS",  "desc":"Revocation Date"                                                 },
   {"type":"CE",  "desc":"Revocation Reason Code"                                          },
   {"type":"CWE", "desc":"Certificate Status"                                              }]},
 "CHOICE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request"                          ,"req":true},
   {"type":"RQD", "desc":"Requisition Detail"                           ,"req":true},
   {"type":"RQ1", "desc":"Requisition Detail - Additional Information"  ,"req":true},
   {"type":"RXO", "desc":"Pharmacy/Treatment Order"                     ,"req":true},
   {"type":"ODS", "desc":"Dietary Orders, Supplements, And Preferences" ,"req":true},
   {"type":"ODT", "desc":"Diet Tray Instructions"                       ,"req":true}]},
 "CHOICE_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request"                          ,"req":true},
   {"type":"RQD", "desc":"Requisition Detail"                           ,"req":true},
   {"type":"RQ1", "desc":"Requisition Detail - Additional Information"  ,"req":true},
   {"type":"RXO", "desc":"Pharmacy/Treatment Order"                     ,"req":true},
   {"type":"ODS", "desc":"Dietary Orders, Supplements, And Preferences" ,"req":true},
   {"type":"ODT", "desc":"Diet Tray Instructions"                       ,"req":true}]},
 "CHOICE_ORDER_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "CLOCK":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NCK", "desc":"System Clock"       ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "CLOCK_AND_STATISTICS":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NCK", "desc":"System Clock"                        },
   {"type":"NST", "desc":"Application Control Level Statistics"},
   {"type":"NSC", "desc":"Application Status Change"           }]},
 "CLOCK_AND_STATS_WITH_NOTES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CLOCK",      "desc":""},
   {"type":"APP_STATS",  "desc":""},
   {"type":"APP_STATUS", "desc":""}]},
 "CLOCK_AND_STATS_WITH_NOTES_ALT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NCK", "desc":"System Clock"                        },
   {"type":"NTE", "desc":"Notes And Comments"                   ,"repeats":true},
   {"type":"NST", "desc":"Application Control Level Statistics"},
   {"type":"NTE", "desc":"Notes And Comments"                   ,"repeats":true},
   {"type":"NSC", "desc":"Application Status Change"           },
   {"type":"NTE", "desc":"Notes And Comments"                   ,"repeats":true}]},
 "CM0":{
  "desc":"Clinical Study Master",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Cm0"              },
   {"type":"EI",  "desc":"Sponsor Study ID"           ,"req":true},
   {"type":"EI",  "desc":"Alternate Study ID"         ,"repeats":true},
   {"type":"ST",  "desc":"Title Of Study"             ,"req":true},
   {"type":"XCN", "desc":"Chairman Of Study"          ,"repeats":true},
   {"type":"DT",  "desc":"Last Irb Approval Date"    },
   {"type":"NM",  "desc":"Total Accrual To Date"     },
   {"type":"DT",  "desc":"Last Accrual Date"         },
   {"type":"XCN", "desc":"Contact For Study"          ,"repeats":true},
   {"type":"XTN", "desc":"Contact's Telephone Number"},
   {"type":"XAD", "desc":"Contact's Address"          ,"repeats":true}]},
 "CM1":{
  "desc":"Clinical Study Phase Master",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Cm1"               ,"req":true},
   {"type":"CE", "desc":"Study Phase Identifier"     ,"req":true},
   {"type":"ST", "desc":"Description Of Study Phase" ,"req":true}]},
 "CM2":{
  "desc":"Clinical Study Schedule Master",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set Id- Cm2"                     },
   {"type":"CE", "desc":"Scheduled Time Point"             ,"req":true},
   {"type":"ST", "desc":"Description Of Time Point"       },
   {"type":"CE", "desc":"Events Scheduled This Time Point" ,"req":true ,"repeats":true}]},
 "CNE":{
  "desc":"Coded With No Exceptions",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Identifier"                        },
   {"type":"ST", "desc":"Text"                              },
   {"type":"ID", "desc":"Name Of Coding System"             },
   {"type":"ST", "desc":"Alternate Identifier"              },
   {"type":"ST", "desc":"Alternate Text"                    },
   {"type":"ID", "desc":"Name Of Alternate Coding System"   },
   {"type":"ST", "desc":"Coding System Version ID"          },
   {"type":"ST", "desc":"Alternate Coding System Version ID"},
   {"type":"ST", "desc":"Original Text"                     }]},
 "CNN":{
  "desc":"Composite ID Number And Name Simplified",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"ID Number"                                         },
   {"type":"ST", "desc":"Family Name"                                       },
   {"type":"ST", "desc":"Given Name"                                        },
   {"type":"ST", "desc":"Second And Further Given Names Or Initials Thereof"},
   {"type":"ST", "desc":"Suffix (e.g., Jr Or III)"                          },
   {"type":"ST", "desc":"Prefix (e.g., Dr)"                                 },
   {"type":"IS", "desc":"Degree (e.g., Md"                                  },
   {"type":"IS", "desc":"Source Table"                                      },
   {"type":"IS", "desc":"Assigning Authority - Namespace ID"                },
   {"type":"ST", "desc":"Assigning Authority - Universal ID"                },
   {"type":"ID", "desc":"Assigning Authority - Universal ID Type"           }]},
 "CNS":{
  "desc":"Clear Notification",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Starting Notification Reference Number"},
   {"type":"NM", "desc":"Ending Notification Reference Number"  },
   {"type":"TS", "desc":"Starting Notification Date/time"       },
   {"type":"TS", "desc":"Ending Notification Date/time"         },
   {"type":"CE", "desc":"Starting Notification Code"            },
   {"type":"CE", "desc":"Ending Notification Code"              }]},
 "COMMAND":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ECD",                "desc":"Equipment Command"  ,"req":true},
   {"type":"TQ1",                "desc":"Timing/Quantity"   },
   {"type":"SPECIMEN_CONTAINER", "desc":""                  },
   {"type":"CNS",                "desc":"Clear Notification"}]},
 "COMMAND_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ECD",                                 "desc":"Equipment Command"          ,"req":true},
   {"type":"SPECIMEN_CONTAINER_COMMAND_RESPONSE", "desc":""                          },
   {"type":"ECR",                                 "desc":"Equipment Command Response" ,"req":true}]},
 "COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                "desc":"Common Order"},
   {"type":"TIMING_QUANTITY",    "desc":""             ,"repeats":true},
   {"type":"ORDER_COMMON_ORDER", "desc":""            },
   {"type":"OBSERVATION",        "desc":""             ,"repeats":true}]},
 "COMMON_ORDER_DFTP11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order"},
   {"type":"TIMING_QUANTITY_COMMON_ORDER", "desc":""             ,"repeats":true},
   {"type":"ORDER_COMMON_ORDER_DFTP11",    "desc":""            },
   {"type":"OBSERVATION_COMMON_ORDER",     "desc":""             ,"repeats":true}]},
 "COMMON_ORDER_MDMT01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                 "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER", "desc":""                    ,"repeats":true},
   {"type":"OBR",                 "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                 "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT02", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT03", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT04", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT05", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT06", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT07", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT08", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT09", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT10", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_MDMT11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_MDMT11", "desc":""                    ,"repeats":true},
   {"type":"OBR",                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                        "desc":"Notes And Comments"  ,"repeats":true}]},
 "COMMON_ORDER_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                     "desc":"Common Order"                       ,"req":true},
   {"type":"TIMING_COMMON_ORDER_QUERY_RESPONSE",      "desc":""                                   ,"repeats":true},
   {"type":"ORDER_DETAIL_COMMON_ORDER",               "desc":""                                  },
   {"type":"ENCODED_ORDER",                           "desc":""                                  },
   {"type":"RXD",                                     "desc":"Pharmacy/Treatment Dispense"        ,"req":true},
   {"type":"RXR",                                     "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                                     "desc":"Pharmacy/Treatment Component Order" ,"repeats":true},
   {"type":"OBSERVATION_COMMON_ORDER_QUERY_RESPONSE", "desc":""                                   ,"req":true ,"repeats":true}]},
 "COMMON_ORDER_QUERY_RESPONSE_RSPZ86":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                            "desc":"Common Order" ,"req":true},
   {"type":"TIMING_COMMON_ORDER_QUERY_RESPONSE_RSPZ86",      "desc":""             ,"repeats":true},
   {"type":"ORDER_DETAIL_COMMON_ORDER_QUERY_RESPONSE",       "desc":""            },
   {"type":"ENCODED_ORDER_COMMON_ORDER",                     "desc":""            },
   {"type":"DISPENSE_COMMON_ORDER",                          "desc":""            },
   {"type":"GIVE_COMMON_ORDER",                              "desc":""            },
   {"type":"ADMINISTRATION_COMMON_ORDER",                    "desc":""            },
   {"type":"OBSERVATION_COMMON_ORDER_QUERY_RESPONSE_RSPZ86", "desc":""             ,"req":true ,"repeats":true}]},
 "COMMON_ORDER_QUERY_RESPONSE_RSPZ88":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                             "desc":"Common Order"                       ,"req":true},
   {"type":"TIMING_COMMON_ORDER_QUERY_RESPONSE_RSPZ88",       "desc":""                                   ,"repeats":true},
   {"type":"ORDER_DETAIL_COMMON_ORDER_QUERY_RESPONSE_RSPZ88", "desc":""                                  },
   {"type":"ORDER_ENCODED",                                   "desc":""                                  },
   {"type":"RXD",                                             "desc":"Pharmacy/Treatment Dispense"        ,"req":true},
   {"type":"RXR",                                             "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                                             "desc":"Pharmacy/Treatment Component Order" ,"repeats":true},
   {"type":"OBSERVATION_COMMON_ORDER_QUERY_RESPONSE_RSPZ88",  "desc":""                                   ,"req":true ,"repeats":true}]},
 "COMMON_ORDER_QUERY_RESPONSE_RSPZ90":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                            "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_COMMON_ORDER_QUERY_RESPONSE_RSPZ90",      "desc":""                    ,"repeats":true},
   {"type":"OBR",                                            "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                                            "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"CTD",                                            "desc":"Contact Data"       },
   {"type":"OBSERVATION_COMMON_ORDER_QUERY_RESPONSE_RSPZ90", "desc":""                    ,"req":true ,"repeats":true}]},
 "COMPONENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENTS":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENTS_ORDER_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENTS_ORDER_DETAIL_SUPPLEMENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENT_ORDER_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENT_ORDER_DETAIL_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true ,"repeats":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENT_ORDER_DETAIL_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENT_ORDER_DETAIL_ORDER_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "COMPONENT_ORDER_DETAIL_SUPPLEMENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "CON":{
  "desc":"Consent Segment",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Con"                              ,"req":true},
   {"type":"CWE", "desc":"Consent Type"                             },
   {"type":"ST",  "desc":"Consent Form ID"                          },
   {"type":"EI",  "desc":"Consent Form Number"                      },
   {"type":"FT",  "desc":"Consent Text"                              ,"repeats":true},
   {"type":"FT",  "desc":"Subject-specific Consent Text"             ,"repeats":true},
   {"type":"FT",  "desc":"Consent Background"                        ,"repeats":true},
   {"type":"FT",  "desc":"Subject-specific Consent Background"       ,"repeats":true},
   {"type":"FT",  "desc":"Consenter-imposed Limitations"             ,"repeats":true},
   {"type":"CNE", "desc":"Consent Mode"                             },
   {"type":"CNE", "desc":"Consent Status"                            ,"req":true},
   {"type":"TS",  "desc":"Consent Discussion Date/time"             },
   {"type":"TS",  "desc":"Consent Decision Date/time"               },
   {"type":"TS",  "desc":"Consent Effective Date/time"              },
   {"type":"TS",  "desc":"Consent End Date/time"                    },
   {"type":"ID",  "desc":"Subject Competence Indicator"             },
   {"type":"ID",  "desc":"Translator Assistance Indicator"          },
   {"type":"ID",  "desc":"Language Translated To"                   },
   {"type":"ID",  "desc":"Informational Material Supplied Indicator"},
   {"type":"CWE", "desc":"Consent Bypass Reason"                    },
   {"type":"ID",  "desc":"Consent Disclosure Level"                 },
   {"type":"CWE", "desc":"Consent Non-disclosure Reason"            },
   {"type":"CWE", "desc":"Non-subject Consenter Reason"             },
   {"type":"XPN", "desc":"Consenter ID"                              ,"req":true ,"repeats":true},
   {"type":"IS",  "desc":"Relationship To Subject Table"             ,"req":true ,"repeats":true}]},
 "CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"OBX", "desc":"Observation/Result"        ,"repeats":true}]},
 "CONTAINER_Group3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC",             "desc":"Specimen Container Detail" ,"req":true},
   {"type":"INV",             "desc":"Inventory Detail"         },
   {"type":"ORDER_CONTAINER", "desc":""                          ,"req":true ,"repeats":true}]},
 "CONTAINER_ORDER_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"SID", "desc":"Substance Identifier"     }]},
 "CONTAINER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"OBX", "desc":"Observation/Result"        ,"repeats":true}]},
 "CONTAINER_SPECIMEN_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"INV", "desc":"Inventory Detail"         }]},
 "CONTAINER_SPECIMEN_OULR22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"INV", "desc":"Inventory Detail"         }]},
 "CP":{
  "desc":"Composite Price",
  "type":"composite",
  "children":[
   {"type":"MO", "desc":"Price"      },
   {"type":"ID", "desc":"Price Type" },
   {"type":"NM", "desc":"From Value" },
   {"type":"NM", "desc":"To Value"   },
   {"type":"CE", "desc":"Range Units"},
   {"type":"ID", "desc":"Range Type" }]},
 "CQ":{
  "desc":"Composite Quantity With Units",
  "type":"composite",
  "children":[
   {"type":"NM", "desc":"Quantity"},
   {"type":"CE", "desc":"Units"   }]},
 "CRMC01":{
  "desc":"Register a patient on a clinical trial",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC01", "desc":""                 ,"req":true ,"repeats":true}]},
 "CRMC02":{
  "desc":"Cancel a patient registration on clinical trial",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC02", "desc":""                 ,"req":true ,"repeats":true}]},
 "CRMC03":{
  "desc":"Correct/update registration information",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC03", "desc":""                 ,"req":true ,"repeats":true}]},
 "CRMC04":{
  "desc":"Patient has gone off a clinical trial",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC04", "desc":""                 ,"req":true ,"repeats":true}]},
 "CRMC05":{
  "desc":"Patient enters phase of clinical trial",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC05", "desc":""                 ,"req":true ,"repeats":true}]},
 "CRMC06":{
  "desc":"Cancel patient entering a phase (clerical mistake)",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC06", "desc":""                 ,"req":true ,"repeats":true}]},
 "CRMC07":{
  "desc":"Correct/update phase information",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC07", "desc":""                 ,"req":true ,"repeats":true}]},
 "CRMC08":{
  "desc":"Patient has gone off phase of clinical trial",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CRMC08", "desc":""                 ,"req":true ,"repeats":true}]},
 "CSP":{
  "desc":"Clinical Study Phase",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Study Phase Identifier"      ,"req":true},
   {"type":"TS", "desc":"Date/time Study Phase Began" ,"req":true},
   {"type":"TS", "desc":"Date/time Study Phase Ended"},
   {"type":"CE", "desc":"Study Phase Evaluability"   }]},
 "CSR":{
  "desc":"Clinical Study Registration",
  "type":"segment",
  "children":[
   {"type":"EI",  "desc":"Sponsor Study ID"                        ,"req":true},
   {"type":"EI",  "desc":"Alternate Study ID"                     },
   {"type":"CE",  "desc":"Institution Registering The Patient"    },
   {"type":"CX",  "desc":"Sponsor Patient ID"                      ,"req":true},
   {"type":"CX",  "desc":"Alternate Patient ID - Csr"             },
   {"type":"TS",  "desc":"Date/time Of Patient Study Registration" ,"req":true},
   {"type":"XCN", "desc":"Person Performing Study Registration"    ,"repeats":true},
   {"type":"XCN", "desc":"Study Authorizing Provider"              ,"req":true ,"repeats":true},
   {"type":"TS",  "desc":"Date/time Patient Study Consent Signed" },
   {"type":"CE",  "desc":"Patient Study Eligibility Status"       },
   {"type":"TS",  "desc":"Study Randomization Date/time"           ,"repeats":true},
   {"type":"CE",  "desc":"Randomized Study Arm"                    ,"repeats":true},
   {"type":"CE",  "desc":"Stratum For Study Randomization"         ,"repeats":true},
   {"type":"CE",  "desc":"Patient Evaluability Status"            },
   {"type":"TS",  "desc":"Date/time Ended Study"                  },
   {"type":"CE",  "desc":"Reason Ended Study"                     }]},
 "CSS":{
  "desc":"Clinical Study Data Schedule Segment",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Study Scheduled Time Point"         ,"req":true},
   {"type":"TS", "desc":"Study Scheduled Patient Time Point"},
   {"type":"CE", "desc":"Study Quality Control Codes"        ,"repeats":true}]},
 "CSUC09":{
  "desc":"Automated time intervals for reporting, like monthly",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CSUC09", "desc":""                 ,"req":true ,"repeats":true}]},
 "CSUC10":{
  "desc":"Patient completes the clinical trial",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CSUC10", "desc":""                 ,"req":true ,"repeats":true}]},
 "CSUC11":{
  "desc":"Patient completes a phase of the clinical trial",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CSUC11", "desc":""                 ,"req":true ,"repeats":true}]},
 "CSUC12":{
  "desc":"Update/correction of patient order/result information",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"   ,"req":true},
   {"type":"SFT",            "desc":"Software Segment" ,"repeats":true},
   {"type":"PATIENT_CSUC12", "desc":""                 ,"req":true ,"repeats":true}]},
 "CTD":{
  "desc":"Contact Data",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Contact Role"                      ,"req":true ,"repeats":true},
   {"type":"XPN", "desc":"Contact Name"                      ,"repeats":true},
   {"type":"XAD", "desc":"Contact Address"                   ,"repeats":true},
   {"type":"PL",  "desc":"Contact Location"                 },
   {"type":"XTN", "desc":"Contact Communication Information" ,"repeats":true},
   {"type":"CE",  "desc":"Preferred Method Of Contact"      },
   {"type":"PLN", "desc":"Contact Identifiers"               ,"repeats":true}]},
 "CTI":{
  "desc":"Clinical Trial Identification",
  "type":"segment",
  "children":[
   {"type":"EI", "desc":"Sponsor Study ID"           ,"req":true},
   {"type":"CE", "desc":"Study Phase Identifier"    },
   {"type":"CE", "desc":"Study Scheduled Time Point"}]},
 "CWE":{
  "desc":"Coded With Exceptions",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Identifier"                        },
   {"type":"ST", "desc":"Text"                              },
   {"type":"ID", "desc":"Name Of Coding System"             },
   {"type":"ST", "desc":"Alternate Identifier"              },
   {"type":"ST", "desc":"Alternate Text"                    },
   {"type":"ID", "desc":"Name Of Alternate Coding System"   },
   {"type":"ST", "desc":"Coding System Version ID"          },
   {"type":"ST", "desc":"Alternate Coding System Version ID"},
   {"type":"ST", "desc":"Original Text"                     }]},
 "CX":{
  "desc":"Extended Composite ID With Check Digit",
  "type":"composite",
  "children":[
   {"type":"ST",  "desc":"ID Number"                     },
   {"type":"ST",  "desc":"Check Digit"                   },
   {"type":"ID",  "desc":"Check Digit Scheme"            },
   {"type":"HD",  "desc":"Assigning Authority"           },
   {"type":"ID",  "desc":"Identifier Type Code"          },
   {"type":"HD",  "desc":"Assigning Facility"            },
   {"type":"DT",  "desc":"Effective Date"                },
   {"type":"DT",  "desc":"Expiration Date"               },
   {"type":"CWE", "desc":"Assigning Jurisdiction"        },
   {"type":"CWE", "desc":"Assigning Agency Or Department"}]},
 "DB1":{
  "desc":"Disability",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Db1"                   ,"req":true},
   {"type":"IS", "desc":"Disabled Person Code"          },
   {"type":"CX", "desc":"Disabled Person Identifier"     ,"repeats":true},
   {"type":"ID", "desc":"Disabled Indicator"            },
   {"type":"DT", "desc":"Disability Start Date"         },
   {"type":"DT", "desc":"Disability End Date"           },
   {"type":"DT", "desc":"Disability Return To Work Date"},
   {"type":"DT", "desc":"Disability Unable To Work Date"}]},
 "DDI":{
  "desc":"Daily Deductible Information",
  "type":"composite",
  "children":[
   {"type":"NM", "desc":"Delay Days"     },
   {"type":"MO", "desc":"Monetary Amount"},
   {"type":"NM", "desc":"Number Of Days" }]},
 "DEFINITION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"QRD",                "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",                "desc":"Original Style Query Filter"},
   {"type":"PATIENT_DEFINITION", "desc":""                           },
   {"type":"ORDER_DEFINITION",   "desc":""                            ,"req":true ,"repeats":true}]},
 "DEFINITION_RDRRDR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"QRD",                       "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",                       "desc":"Original Style Query Filter"},
   {"type":"PATIENT_DEFINITION_RDRRDR", "desc":""                           },
   {"type":"ORDER_DEFINITION_RDRRDR",   "desc":""                            ,"req":true ,"repeats":true}]},
 "DEFINITION_RERRER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"QRD",                       "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",                       "desc":"Original Style Query Filter"},
   {"type":"PATIENT_DEFINITION_RERRER", "desc":""                           },
   {"type":"ORDER_DEFINITION_RERRER",   "desc":""                            ,"req":true ,"repeats":true}]},
 "DEFINITION_RGRRGR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"QRD",                       "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",                       "desc":"Original Style Query Filter"},
   {"type":"PATIENT_DEFINITION_RGRRGR", "desc":""                           },
   {"type":"ORDER_DEFINITION_RGRRGR",   "desc":""                            ,"req":true ,"repeats":true}]},
 "DEFINITION_RORROR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"QRD",                       "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",                       "desc":"Original Style Query Filter"},
   {"type":"PATIENT_DEFINITION_RORROR", "desc":""                           },
   {"type":"ORDER_DEFINITION_RORROR",   "desc":""                            ,"req":true ,"repeats":true}]},
 "DFTP03":{
  "desc":"Post detail financial transaction",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN",              "desc":"Event Type"                     ,"req":true},
   {"type":"PID",              "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"},
   {"type":"ROL",              "desc":"Role"                           ,"repeats":true},
   {"type":"PV1",              "desc":"Patient Visit"                 },
   {"type":"PV2",              "desc":"Additional Information"        },
   {"type":"ROL",              "desc":"Role"                           ,"repeats":true},
   {"type":"DB1",              "desc":"Disability"                     ,"repeats":true},
   {"type":"COMMON_ORDER",     "desc":""                               ,"repeats":true},
   {"type":"FINANCIAL",        "desc":""                               ,"req":true ,"repeats":true},
   {"type":"DG1",              "desc":"Diagnosis"                      ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"       },
   {"type":"GT1",              "desc":"Guarantor"                      ,"repeats":true},
   {"type":"INSURANCE_DFTP03", "desc":""                               ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                      }]},
 "DFTP11":{
  "desc":"Post detail financial transaction - expanded",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                     ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                 "desc":"Patient Additional Demographic"},
   {"type":"ROL",                 "desc":"Role"                           ,"repeats":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 },
   {"type":"PV2",                 "desc":"Additional Information"        },
   {"type":"ROL",                 "desc":"Role"                           ,"repeats":true},
   {"type":"DB1",                 "desc":"Disability"                     ,"repeats":true},
   {"type":"COMMON_ORDER_DFTP11", "desc":""                               ,"repeats":true},
   {"type":"DG1",                 "desc":"Diagnosis"                      ,"repeats":true},
   {"type":"DRG",                 "desc":"Diagnosis Related Group"       },
   {"type":"GT1",                 "desc":"Guarantor"                      ,"repeats":true},
   {"type":"INSURANCE_DFTP11",    "desc":""                               ,"repeats":true},
   {"type":"ACC",                 "desc":"Accident"                      },
   {"type":"FINANCIAL_DFTP11",    "desc":""                               ,"req":true ,"repeats":true}]},
 "DG1":{
  "desc":"Diagnosis",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Dg1"              ,"req":true},
   {"type":"ID",  "desc":"Diagnosis Coding Method"  },
   {"type":"CE",  "desc":"Diagnosis Code - Dg1"     },
   {"type":"ST",  "desc":"Diagnosis Description"    },
   {"type":"TS",  "desc":"Diagnosis Date/time"      },
   {"type":"IS",  "desc":"Diagnosis Type"            ,"req":true},
   {"type":"CE",  "desc":"Major Diagnostic Category"},
   {"type":"CE",  "desc":"Diagnostic Related Group" },
   {"type":"ID",  "desc":"Drg Approval Indicator"   },
   {"type":"IS",  "desc":"Drg Grouper Review Code"  },
   {"type":"CE",  "desc":"Outlier Type"             },
   {"type":"NM",  "desc":"Outlier Days"             },
   {"type":"CP",  "desc":"Outlier Cost"             },
   {"type":"ST",  "desc":"Grouper Version And Type" },
   {"type":"ID",  "desc":"Diagnosis Priority"       },
   {"type":"XCN", "desc":"Diagnosing Clinician"      ,"repeats":true},
   {"type":"IS",  "desc":"Diagnosis Classification" },
   {"type":"ID",  "desc":"Confidential Indicator"   },
   {"type":"TS",  "desc":"Attestation Date/time"    },
   {"type":"EI",  "desc":"Diagnosis Identifier"     },
   {"type":"ID",  "desc":"Diagnosis Action Code"    }]},
 "DIET":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ODS",              "desc":"Dietary Orders, Supplements, And Preferences" ,"req":true ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"                           ,"repeats":true},
   {"type":"OBSERVATION_DIET", "desc":""                                             ,"repeats":true}]},
 "DIN":{
  "desc":"Date And Institution Name",
  "type":"composite",
  "children":[
   {"type":"TS", "desc":"Date"            },
   {"type":"CE", "desc":"Institution Name"}]},
 "DISPENSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXD", "desc":"Pharmacy/Treatment Dispense"        ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "DISPENSE_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXD", "desc":"Pharmacy/Treatment Dispense"        ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "DISPENSE_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXD", "desc":"Pharmacy/Treatment Dispense"        ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "DLD":{
  "desc":"Discharge Location And Date",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Discharge Location"},
   {"type":"TS", "desc":"Effective Date"    }]},
 "DLN":{
  "desc":"Driver's License Number",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"License Number"                  },
   {"type":"IS", "desc":"Issuing State, Province, Country"},
   {"type":"DT", "desc":"Expiration Date"                 }]},
 "DLT":{
  "desc":"Delta",
  "type":"composite",
  "children":[
   {"type":"NR", "desc":"Normal Range"      },
   {"type":"NM", "desc":"Numeric Threshold" },
   {"type":"ID", "desc":"Change Computation"},
   {"type":"NM", "desc":"Days Retained"     }]},
 "DOCT12":{
  "desc":"Document query - response",
  "type":"message",
  "children":[
   {"type":"MSH",    "desc":"Message Header"         ,"req":true},
   {"type":"MSA",    "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",    "desc":"Error"                 },
   {"type":"QAK",    "desc":"Query Acknowledgment"  },
   {"type":"QRD",    "desc":"Style Query Definition" ,"req":true},
   {"type":"RESULT", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"DSC",    "desc":"Continuation Pointer"  }]},
 "DR":{
  "desc":"Date/Time Range",
  "type":"composite",
  "children":[
   {"type":"TS", "desc":"Range Start Date/time"},
   {"type":"TS", "desc":"Range End Date/time"  }]},
 "DRG":{
  "desc":"Diagnosis Related Group",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Diagnostic Related Group"},
   {"type":"TS", "desc":"Drg Assigned Date/time"  },
   {"type":"ID", "desc":"Drg Approval Indicator"  },
   {"type":"IS", "desc":"Drg Grouper Review Code" },
   {"type":"CE", "desc":"Outlier Type"            },
   {"type":"NM", "desc":"Outlier Days"            },
   {"type":"CP", "desc":"Outlier Cost"            },
   {"type":"IS", "desc":"Drg Payor"               },
   {"type":"CP", "desc":"Outlier Reimbursement"   },
   {"type":"ID", "desc":"Confidential Indicator"  },
   {"type":"IS", "desc":"Drg Transfer Type"       }]},
 "DSC":{
  "desc":"Continuation Pointer",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Continuation Pointer"},
   {"type":"ID", "desc":"Continuation Style"  }]},
 "DSP":{
  "desc":"Display Data",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Dsp"       },
   {"type":"SI", "desc":"Display Level"      },
   {"type":"TX", "desc":"Data Line"           ,"req":true},
   {"type":"ST", "desc":"Logical Break Point"},
   {"type":"TX", "desc":"Result ID"          }]},
 "DT":{
  "desc":"Date",
  "type":"composite"
 },
 "DTM":{
  "desc":"Date/Time",
  "type":"composite"
 },
 "DTN":{
  "desc":"Day Type And Number",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Day Type"      },
   {"type":"NM", "desc":"Number Of Days"}]},
 "EACU07":{
  "desc":"Automated equipment command",
  "type":"message",
  "children":[
   {"type":"MSH",     "desc":"Message Header"   ,"req":true},
   {"type":"SFT",     "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU",     "desc":"Equipment Detail" ,"req":true},
   {"type":"COMMAND", "desc":""                 ,"req":true ,"repeats":true},
   {"type":"ROL",     "desc":"Role"            }]},
 "EANU09":{
  "desc":"Automated equipment notification",
  "type":"message",
  "children":[
   {"type":"MSH",          "desc":"Message Header"   ,"req":true},
   {"type":"SFT",          "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU",          "desc":"Equipment Detail" ,"req":true},
   {"type":"NOTIFICATION", "desc":""                 ,"req":true ,"repeats":true},
   {"type":"ROL",          "desc":"Role"            }]},
 "EARU08":{
  "desc":"Automated equipment response",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU",              "desc":"Equipment Detail" ,"req":true},
   {"type":"COMMAND_RESPONSE", "desc":""                 ,"req":true ,"repeats":true},
   {"type":"ROL",              "desc":"Role"            }]},
 "ECD":{
  "desc":"Equipment Command",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Reference Command Number"  ,"req":true},
   {"type":"CE", "desc":"Remote Control Command"    ,"req":true},
   {"type":"ID", "desc":"Response Required"        },
   {"type":"TQ", "desc":"Requested Completion Time"},
   {"type":"TX", "desc":"Parameters"                ,"repeats":true}]},
 "ECR":{
  "desc":"Equipment Command Response",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Command Response"            ,"req":true},
   {"type":"TS", "desc":"Date/time Completed"         ,"req":true},
   {"type":"TX", "desc":"Command Response Parameters" ,"repeats":true}]},
 "ED":{
  "desc":"Encapsulated Data (Wrong Segment)",
  "type":"segment",
  "children":[
   {"type":"HD", "desc":"Source Application"},
   {"type":"ID", "desc":"Type Of Data"      },
   {"type":"ID", "desc":"Data Subtype"      },
   {"type":"ID", "desc":"Encoding"          },
   {"type":"TX", "desc":"Data"              }]},
 "EDU":{
  "desc":"Educational Detail",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID _ Edu"                                     ,"req":true},
   {"type":"IS",  "desc":"Academic Degree"                                 },
   {"type":"DR",  "desc":"Academic Degree Program Date Range"              },
   {"type":"DR",  "desc":"Academic Degree Program Participation Date Range"},
   {"type":"DT",  "desc":"Academic Degree Granted Date"                    },
   {"type":"XON", "desc":"School"                                          },
   {"type":"CE",  "desc":"School Type Code"                                },
   {"type":"XAD", "desc":"School Address"                                  },
   {"type":"CWE", "desc":"Major Field Of Study"                             ,"repeats":true}]},
 "EI":{
  "desc":"Entity Identifier",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Entity Identifier"},
   {"type":"IS", "desc":"Namespace ID"     },
   {"type":"ST", "desc":"Universal ID"     },
   {"type":"ID", "desc":"Universal ID Type"}]},
 "EIP":{
  "desc":"Entity Identifier Pair",
  "type":"composite",
  "children":[
   {"type":"EI", "desc":"Placer Assigned Identifier"},
   {"type":"EI", "desc":"Filler Assigned Identifier"}]},
 "ELD":{
  "desc":"Error Location And Description",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Segment ID"            },
   {"type":"NM", "desc":"Segment Sequence"      },
   {"type":"NM", "desc":"Field Position"        },
   {"type":"CE", "desc":"Code Identifying Error"}]},
 "ENCODED_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                          "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"TIMING_ENCODED_ENCODED_ORDER", "desc":""                                   ,"repeats":true},
   {"type":"RXR",                          "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                          "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODED_ORDER_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                                       "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"TIMING_ENCODED_ENCODED_ORDER_COMMON_ORDER", "desc":""                                   ,"repeats":true},
   {"type":"RXR",                                       "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                                       "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE", "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",            "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"TIMING_ENCODED", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",            "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",            "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER_DEFINITION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE", "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER_DEFINITION_RGRRGR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE", "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER_RDSO13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                     "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"NTE",                     "desc":"Notes And Comments"                 ,"repeats":true},
   {"type":"TIMING_ENCODED_ENCODING", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",                     "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                     "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                                    "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"NTE",                                    "desc":"Notes And Comments"                 ,"repeats":true},
   {"type":"TIMING_ENCODED_ENCODING_ORDER_RESPONSE", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",                                    "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                                    "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER_RESPONSE_RREO26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                                           "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"NTE",                                           "desc":"Notes And Comments"                 ,"repeats":true},
   {"type":"TIMING_ENCODED_ENCODING_ORDER_RESPONSE_RREO26", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",                                           "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                                           "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER_RESPONSE_RSPK31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                                           "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"TIMING_ENCODED_ENCODING_ORDER_RESPONSE_RSPK31", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",                                           "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                                           "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ENCODING_ORDER_RGVO15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                           "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"TIMING_ENCODED_ENCODING_ORDER", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",                           "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                           "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "EQL":{
  "desc":"Embedded Query Language",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Query Tag"                 },
   {"type":"ID", "desc":"Query/response Format Code" ,"req":true},
   {"type":"CE", "desc":"Eql Query Name"             ,"req":true},
   {"type":"ST", "desc":"Eql Query Statement"        ,"req":true}]},
 "EQP":{
  "desc":"Equipment/Log Service",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Event Type"       ,"req":true},
   {"type":"ST", "desc":"File Name"       },
   {"type":"TS", "desc":"Start Date/time"  ,"req":true},
   {"type":"TS", "desc":"End Date/time"   },
   {"type":"FT", "desc":"Transaction Data" ,"req":true}]},
 "EQU":{
  "desc":"Equipment Detail",
  "type":"segment",
  "children":[
   {"type":"EI", "desc":"Equipment Instance Identifier" ,"req":true},
   {"type":"TS", "desc":"Event Date/time"               ,"req":true},
   {"type":"CE", "desc":"Equipment State"              },
   {"type":"CE", "desc":"Local/remote Control State"   },
   {"type":"CE", "desc":"Alert Level"                  }]},
 "ERL":{
  "desc":"Error Location",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Segment ID"          },
   {"type":"NM", "desc":"Segment Sequence"    },
   {"type":"NM", "desc":"Field Position"      },
   {"type":"NM", "desc":"Field Repetition"    },
   {"type":"NM", "desc":"Component Number"    },
   {"type":"NM", "desc":"Sub-component Number"}]},
 "ERQ":{
  "desc":"Event Replay Query",
  "type":"segment",
  "children":[
   {"type":"ST",  "desc":"Query Tag"           },
   {"type":"CE",  "desc":"Event Identifier"     ,"req":true},
   {"type":"QIP", "desc":"Input Parameter List" ,"repeats":true}]},
 "ERR":{
  "desc":"Error",
  "type":"segment",
  "children":[
   {"type":"ELD", "desc":"Error Code And Location"     ,"repeats":true},
   {"type":"ERL", "desc":"Error Location"              ,"repeats":true},
   {"type":"CWE", "desc":"Hl7 Error Code"              ,"req":true},
   {"type":"ID",  "desc":"Severity"                    ,"req":true},
   {"type":"CWE", "desc":"Application Error Code"     },
   {"type":"ST",  "desc":"Application Error Parameter" ,"repeats":true},
   {"type":"TX",  "desc":"Diagnostic Information"     },
   {"type":"TX",  "desc":"User Message"               },
   {"type":"IS",  "desc":"Inform Person Indicator"     ,"repeats":true},
   {"type":"CWE", "desc":"Override Type"              },
   {"type":"CWE", "desc":"Override Reason Code"        ,"repeats":true},
   {"type":"XTN", "desc":"Help Desk Contact Point"     ,"repeats":true}]},
 "ESRU02":{
  "desc":"Automated equipment status request",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"   ,"req":true},
   {"type":"SFT", "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU", "desc":"Equipment Detail" ,"req":true},
   {"type":"ROL", "desc":"Role"            }]},
 "ESUU01":{
  "desc":"Automated equipment status update",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"            ,"req":true},
   {"type":"SFT", "desc":"Software Segment"          ,"repeats":true},
   {"type":"EQU", "desc":"Equipment Detail"          ,"req":true},
   {"type":"ISD", "desc":"Interaction Status Detail" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                     }]},
 "EVN":{
  "desc":"Event Type",
  "type":"segment",
  "children":[
   {"type":"ID",  "desc":"Event Type Code"        },
   {"type":"TS",  "desc":"Recorded Date/time"      ,"req":true},
   {"type":"TS",  "desc":"Date/time Planned Event"},
   {"type":"IS",  "desc":"Event Reason Code"      },
   {"type":"XCN", "desc":"Operator ID"             ,"repeats":true},
   {"type":"TS",  "desc":"Event Occurred"         },
   {"type":"HD",  "desc":"Event Facility"         }]},
 "EXPERIENCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PES",             "desc":"Product Experience Sender" ,"req":true},
   {"type":"PEX_OBSERVATION", "desc":""                          ,"req":true ,"repeats":true}]},
 "EXPERIENCE_PEXP08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PES",                        "desc":"Product Experience Sender" ,"req":true},
   {"type":"PEX_OBSERVATION_EXPERIENCE", "desc":""                          ,"req":true ,"repeats":true}]},
 "FAC":{
  "desc":"Facility",
  "type":"segment",
  "children":[
   {"type":"EI",  "desc":"Facility Id-fac"                       ,"req":true},
   {"type":"ID",  "desc":"Facility Type"                        },
   {"type":"XAD", "desc":"Facility Address"                      ,"req":true ,"repeats":true},
   {"type":"XTN", "desc":"Facility Telecommunication"            ,"req":true},
   {"type":"XCN", "desc":"Contact Person"                        ,"repeats":true},
   {"type":"ST",  "desc":"Contact Title"                         ,"repeats":true},
   {"type":"XAD", "desc":"Contact Address"                       ,"repeats":true},
   {"type":"XTN", "desc":"Contact Telecommunication"             ,"repeats":true},
   {"type":"XCN", "desc":"Signature Authority"                   ,"req":true ,"repeats":true},
   {"type":"ST",  "desc":"Signature Authority Title"            },
   {"type":"XAD", "desc":"Signature Authority Address"           ,"repeats":true},
   {"type":"XTN", "desc":"Signature Authority Telecommunication"}]},
 "FACILITY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"FAC",              "desc":"Facility"                          ,"req":true},
   {"type":"PRODUCT_FACILITY", "desc":""                                  ,"req":true ,"repeats":true},
   {"type":"PSH",              "desc":"Product Summary Header"            ,"req":true},
   {"type":"FACILITY_DETAIL",  "desc":""                                  ,"req":true ,"repeats":true},
   {"type":"ED",               "desc":"Encapsulated Data (Wrong Segment)" ,"req":true}]},
 "FACILITY_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"FAC", "desc":"Facility"               ,"req":true},
   {"type":"PDC", "desc":"Product Detail Country" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"req":true}]},
 "FC":{
  "desc":"Financial Class",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Financial Class Code"},
   {"type":"TS", "desc":"Effective Date"      }]},
 "FHS":{
  "desc":"File Header",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"File Field Separator"       ,"req":true},
   {"type":"ST", "desc":"File Encoding Characters"   ,"req":true},
   {"type":"HD", "desc":"File Sending Application"  },
   {"type":"HD", "desc":"File Sending Facility"     },
   {"type":"HD", "desc":"File Receiving Application"},
   {"type":"HD", "desc":"File Receiving Facility"   },
   {"type":"TS", "desc":"File Creation Date/time"   },
   {"type":"ST", "desc":"File Security"             },
   {"type":"ST", "desc":"File Name/id"              },
   {"type":"ST", "desc":"File Header Comment"       },
   {"type":"ST", "desc":"File Control ID"           },
   {"type":"ST", "desc":"Reference File Control ID" }]},
 "FINANCIAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"FT1",                    "desc":"Financial Transaction" ,"req":true},
   {"type":"NTE",                    "desc":"Notes And Comments"   },
   {"type":"FINANCIAL_PROCEDURE",    "desc":""                      ,"repeats":true},
   {"type":"FINANCIAL_COMMON_ORDER", "desc":""                      ,"repeats":true}]},
 "FINANCIAL_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                       "desc":"Common Order"},
   {"type":"FINANCIAL_TIMING_QUANTITY", "desc":""             ,"repeats":true},
   {"type":"FINANCIAL_ORDER",           "desc":""            },
   {"type":"FINANCIAL_OBSERVATION",     "desc":""             ,"repeats":true}]},
 "FINANCIAL_COMMON_ORDER_FINANCIAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                              "desc":"Common Order"},
   {"type":"FINANCIAL_TIMING_QUANTITY_FINANCIAL_COMMON_ORDER", "desc":""             ,"repeats":true},
   {"type":"FINANCIAL_ORDER_FINANCIAL_COMMON_ORDER",           "desc":""            },
   {"type":"FINANCIAL_OBSERVATION_FINANCIAL_COMMON_ORDER",     "desc":""             ,"repeats":true}]},
 "FINANCIAL_DFTP11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"FT1",                              "desc":"Financial Transaction"   ,"req":true},
   {"type":"FINANCIAL_PROCEDURE_FINANCIAL",    "desc":""                        ,"repeats":true},
   {"type":"FINANCIAL_COMMON_ORDER_FINANCIAL", "desc":""                        ,"repeats":true},
   {"type":"DG1",                              "desc":"Diagnosis"               ,"repeats":true},
   {"type":"DRG",                              "desc":"Diagnosis Related Group"},
   {"type":"GT1",                              "desc":"Guarantor"               ,"repeats":true},
   {"type":"FINANCIAL_INSURANCE",              "desc":""                        ,"repeats":true}]},
 "FINANCIAL_INSURANCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "FINANCIAL_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "FINANCIAL_OBSERVATION_FINANCIAL_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "FINANCIAL_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"  ,"repeats":true}]},
 "FINANCIAL_ORDER_FINANCIAL_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"  ,"repeats":true}]},
 "FINANCIAL_PROCEDURE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "FINANCIAL_PROCEDURE_FINANCIAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "FINANCIAL_TIMING_QUANTITY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "FINANCIAL_TIMING_QUANTITY_FINANCIAL_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "FN":{
  "desc":"Family Name",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Surname"                           },
   {"type":"ST", "desc":"Own Surname Prefix"                },
   {"type":"ST", "desc":"Own Surname"                       },
   {"type":"ST", "desc":"Surname Prefix From Partner/spouse"},
   {"type":"ST", "desc":"Surname From Partner/spouse"       }]},
 "FT":{
  "desc":"Formatted Text Data",
  "type":"composite"
 },
 "FT1":{
  "desc":"Financial Transaction",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Ft1"                                   },
   {"type":"ST",  "desc":"Transaction ID"                                 },
   {"type":"ST",  "desc":"Transaction Batch ID"                           },
   {"type":"DR",  "desc":"Transaction Date"                                ,"req":true},
   {"type":"TS",  "desc":"Transaction Posting Date"                       },
   {"type":"IS",  "desc":"Transaction Type"                                ,"req":true},
   {"type":"CE",  "desc":"Transaction Code"                                ,"req":true},
   {"type":"ST",  "desc":"Transaction Description"                        },
   {"type":"ST",  "desc":"Transaction Description - Alt"                  },
   {"type":"NM",  "desc":"Transaction Quantity"                           },
   {"type":"CP",  "desc":"Transaction Amount - Extended"                  },
   {"type":"CP",  "desc":"Transaction Amount - Unit"                      },
   {"type":"CE",  "desc":"Department Code"                                },
   {"type":"CE",  "desc":"Insurance Plan ID"                              },
   {"type":"CP",  "desc":"Insurance Amount"                               },
   {"type":"PL",  "desc":"Assigned Patient Location"                      },
   {"type":"IS",  "desc":"Fee Schedule"                                   },
   {"type":"IS",  "desc":"Patient Type"                                   },
   {"type":"CE",  "desc":"Diagnosis Code - Ft1"                            ,"repeats":true},
   {"type":"XCN", "desc":"Performed By Code"                               ,"repeats":true},
   {"type":"XCN", "desc":"Ordered By Code"                                 ,"repeats":true},
   {"type":"CP",  "desc":"Unit Cost"                                      },
   {"type":"EI",  "desc":"Filler Order Number"                            },
   {"type":"XCN", "desc":"Entered By Code"                                 ,"repeats":true},
   {"type":"CE",  "desc":"Procedure Code"                                 },
   {"type":"CE",  "desc":"Procedure Code Modifier"                         ,"repeats":true},
   {"type":"CE",  "desc":"Advanced Beneficiary Notice Code"               },
   {"type":"CWE", "desc":"Medically Necessary Duplicate Procedure Reason."},
   {"type":"CNE", "desc":"Ndc Code"                                       },
   {"type":"CX",  "desc":"Payment Reference ID"                           },
   {"type":"SI",  "desc":"Transaction Reference Key"                       ,"repeats":true}]},
 "FTS":{
  "desc":"File Trailer",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"File Batch Count"    },
   {"type":"ST", "desc":"File Trailer Comment"}]},
 "GENERAL_RESOURCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   }]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SIUS26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GENERAL_RESOURCE_RESOURCES_SRMS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIG", "desc":"Appointment Information - General Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                   },
   {"type":"NTE", "desc":"Notes And Comments"                         ,"repeats":true}]},
 "GIVE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXG",              "desc":"Pharmacy/Treatment Give"            ,"req":true},
   {"type":"TIMING_GIVE",      "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",              "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",              "desc":"Pharmacy/Treatment Component Order" ,"repeats":true},
   {"type":"OBSERVATION_GIVE", "desc":""                                   ,"req":true ,"repeats":true}]},
 "GIVE_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXG", "desc":"Pharmacy/Treatment Give"            ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "GIVE_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXG",              "desc":"Pharmacy/Treatment Give"            ,"req":true},
   {"type":"TIMING_GIVE_GIVE", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",              "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",              "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",              "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",              "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",              "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE",        "desc":""                   ,"repeats":true},
   {"type":"PATHWAY",          "desc":""                   ,"repeats":true},
   {"type":"OBSERVATION_GOAL", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM",          "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL",       "desc":""                   ,"repeats":true}]},
 "GOAL_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_OBSERVATION_GOAL_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                    "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                    "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                    "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PATHWAY", "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION",       "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_GOAL_PATHWAY",   "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL_PATHWAY",     "desc":""                   ,"repeats":true}]},
 "GOAL_PATHWAY_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                   "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                   "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                   "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PATHWAY_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PATHWAY_PATIENT", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_GOAL_PATHWAY_PATIENT",          "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL_PATHWAY_PATIENT",            "desc":""                   ,"repeats":true}]},
 "GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                           "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                           "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                           "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PATHWAY_PPGPCH", "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL",         "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_GOAL_PATHWAY_PPGPCH",   "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL_PATHWAY_PPGPCH",     "desc":""                   ,"repeats":true}]},
 "GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                           "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                           "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                           "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PATHWAY_PPGPCJ", "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PATHWAY", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_GOAL_PATHWAY_PPGPCJ",   "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL_PATHWAY_PPGPCJ",     "desc":""                   ,"repeats":true}]},
 "GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                           "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                           "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                           "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_PATHWAY_GOAL",             "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PATIENT", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_GOAL_PATIENT",          "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL_PATIENT",            "desc":""                   ,"repeats":true}]},
 "GOAL_PGLPC7":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                     "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                     "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                     "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL",          "desc":""                   ,"repeats":true},
   {"type":"PATHWAY_GOAL",            "desc":""                   ,"repeats":true},
   {"type":"OBSERVATION_GOAL_PGLPC7", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_GOAL",            "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL_PGLPC7",       "desc":""                   ,"repeats":true}]},
 "GOAL_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                     "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                     "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                     "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PGLPC8",   "desc":""                   ,"repeats":true},
   {"type":"PATHWAY_GOAL_PGLPC8",     "desc":""                   ,"repeats":true},
   {"type":"OBSERVATION_GOAL_PGLPC8", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_GOAL_PGLPC8",     "desc":""                   ,"repeats":true},
   {"type":"ORDER_GOAL_PGLPC8",       "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                           "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                           "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                           "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM", "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                   "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                   "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                   "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM_PATHWAY",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM_PATHWAY", "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                           "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                           "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                           "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM_PATHWAY_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM_PATHWAY_PATIENT", "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                          "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                          "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                          "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM_PATHWAY_PPPPCD",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM_PATHWAY_PPPPCD", "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                   "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                   "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                   "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM_PATIENT", "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                  "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                  "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                  "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM_PPRPC1",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM_PPRPC1", "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                  "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                  "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                  "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM_PPRPC2",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM_PPRPC2", "desc":""                   ,"repeats":true}]},
 "GOAL_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GOL",                                  "desc":"Goal Detail"        ,"req":true},
   {"type":"NTE",                                  "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                  "desc":"Variance"           ,"repeats":true},
   {"type":"GOAL_ROLE_GOAL_PROBLEM_PPRPC3",        "desc":""                   ,"repeats":true},
   {"type":"GOAL_OBSERVATION_GOAL_PROBLEM_PPRPC3", "desc":""                   ,"repeats":true}]},
 "GOAL_ROLE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOAL_ROLE_GOAL_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "GOL":{
  "desc":"Goal Detail",
  "type":"segment",
  "children":[
   {"type":"ID",  "desc":"Action Code"                      ,"req":true},
   {"type":"TS",  "desc":"Action Date/time"                 ,"req":true},
   {"type":"CE",  "desc":"Goal ID"                          ,"req":true},
   {"type":"EI",  "desc":"Goal Instance ID"                 ,"req":true},
   {"type":"EI",  "desc":"Episode Of Care ID"              },
   {"type":"NM",  "desc":"Goal List Priority"              },
   {"type":"TS",  "desc":"Goal Established Date/time"      },
   {"type":"TS",  "desc":"Expected Goal Achieve Date/time" },
   {"type":"CE",  "desc":"Goal Classification"             },
   {"type":"CE",  "desc":"Goal Management Discipline"      },
   {"type":"CE",  "desc":"Current Goal Review Status"      },
   {"type":"TS",  "desc":"Current Goal Review Date/time"   },
   {"type":"TS",  "desc":"Next Goal Review Date/time"      },
   {"type":"TS",  "desc":"Previous Goal Review Date/time"  },
   {"type":"TQ",  "desc":"Goal Review Interval"            },
   {"type":"CE",  "desc":"Goal Evaluation"                 },
   {"type":"ST",  "desc":"Goal Evaluation Comment"          ,"repeats":true},
   {"type":"CE",  "desc":"Goal Life Cycle Status"          },
   {"type":"TS",  "desc":"Goal Life Cycle Status Date/time"},
   {"type":"CE",  "desc":"Goal Target Type"                 ,"repeats":true},
   {"type":"XPN", "desc":"Goal Target Name"                 ,"repeats":true}]},
 "GP1":{
  "desc":"Visit",
  "type":"segment",
  "children":[
   {"type":"IS", "desc":"Type Of Bill Code"              ,"req":true},
   {"type":"IS", "desc":"Revenue Code"                   ,"repeats":true},
   {"type":"IS", "desc":"Overall Claim Disposition Code"},
   {"type":"IS", "desc":"Oce Edits Per Visit Code"       ,"repeats":true},
   {"type":"CP", "desc":"Outlier Cost"                  }]},
 "GP2":{
  "desc":"Procedure Line Item",
  "type":"segment",
  "children":[
   {"type":"IS", "desc":"Revenue Code"                          },
   {"type":"NM", "desc":"Number Of Service Units"               },
   {"type":"CP", "desc":"Charge"                                },
   {"type":"IS", "desc":"Reimbursement Action Code"             },
   {"type":"IS", "desc":"Denial Or Rejection Code"              },
   {"type":"IS", "desc":"Oce Edit Code"                          ,"repeats":true},
   {"type":"CE", "desc":"Ambulatory Payment Classification Code"},
   {"type":"IS", "desc":"Modifier Edit Code"                     ,"repeats":true},
   {"type":"IS", "desc":"Payment Adjustment Code"               },
   {"type":"IS", "desc":"Packaging Status Code"                 },
   {"type":"CP", "desc":"Expected Cms Payment Amount"           },
   {"type":"IS", "desc":"Reimbursement Type Code"               },
   {"type":"CP", "desc":"Co-pay Amount"                         },
   {"type":"NM", "desc":"Pay Rate Per Service Unit"             }]},
 "GT1":{
  "desc":"Guarantor",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Gt1"                           ,"req":true},
   {"type":"CX",  "desc":"Guarantor Number"                       ,"repeats":true},
   {"type":"XPN", "desc":"Guarantor Name"                         ,"req":true ,"repeats":true},
   {"type":"XPN", "desc":"Guarantor Spouse Name"                  ,"repeats":true},
   {"type":"XAD", "desc":"Guarantor Address"                      ,"repeats":true},
   {"type":"XTN", "desc":"Guarantor Ph Num - Home"                ,"repeats":true},
   {"type":"XTN", "desc":"Guarantor Ph Num - Business"            ,"repeats":true},
   {"type":"TS",  "desc":"Guarantor Date/time Of Birth"          },
   {"type":"IS",  "desc":"Guarantor Administrative Sex"          },
   {"type":"IS",  "desc":"Guarantor Type"                        },
   {"type":"CE",  "desc":"Guarantor Relationship"                },
   {"type":"ST",  "desc":"Guarantor Ssn"                         },
   {"type":"DT",  "desc":"Guarantor Date - Begin"                },
   {"type":"DT",  "desc":"Guarantor Date - End"                  },
   {"type":"NM",  "desc":"Guarantor Priority"                    },
   {"type":"XPN", "desc":"Guarantor Employer Name"                ,"repeats":true},
   {"type":"XAD", "desc":"Guarantor Employer Address"             ,"repeats":true},
   {"type":"XTN", "desc":"Guarantor Employer Phone Number"        ,"repeats":true},
   {"type":"CX",  "desc":"Guarantor Employee ID Number"           ,"repeats":true},
   {"type":"IS",  "desc":"Guarantor Employment Status"           },
   {"type":"XON", "desc":"Guarantor Organization Name"            ,"repeats":true},
   {"type":"ID",  "desc":"Guarantor Billing Hold Flag"           },
   {"type":"CE",  "desc":"Guarantor Credit Rating Code"          },
   {"type":"TS",  "desc":"Guarantor Death Date And Time"         },
   {"type":"ID",  "desc":"Guarantor Death Flag"                  },
   {"type":"CE",  "desc":"Guarantor Charge Adjustment Code"      },
   {"type":"CP",  "desc":"Guarantor Household Annual Income"     },
   {"type":"NM",  "desc":"Guarantor Household Size"              },
   {"type":"CX",  "desc":"Guarantor Employer ID Number"           ,"repeats":true},
   {"type":"CE",  "desc":"Guarantor Marital Status Code"         },
   {"type":"DT",  "desc":"Guarantor Hire Effective Date"         },
   {"type":"DT",  "desc":"Employment Stop Date"                  },
   {"type":"IS",  "desc":"Living Dependency"                     },
   {"type":"IS",  "desc":"Ambulatory Status"                      ,"repeats":true},
   {"type":"CE",  "desc":"Citizenship"                            ,"repeats":true},
   {"type":"CE",  "desc":"Primary Language"                      },
   {"type":"IS",  "desc":"Living Arrangement"                    },
   {"type":"CE",  "desc":"Publicity Code"                        },
   {"type":"ID",  "desc":"Protection Indicator"                  },
   {"type":"IS",  "desc":"Student Indicator"                     },
   {"type":"CE",  "desc":"Religion"                              },
   {"type":"XPN", "desc":"Mother's Maiden Name"                   ,"repeats":true},
   {"type":"CE",  "desc":"Nationality"                           },
   {"type":"CE",  "desc":"Ethnic Group"                           ,"repeats":true},
   {"type":"XPN", "desc":"Contact Person's Name"                  ,"repeats":true},
   {"type":"XTN", "desc":"Contact Person's Telephone Number"      ,"repeats":true},
   {"type":"CE",  "desc":"Contact Reason"                        },
   {"type":"IS",  "desc":"Contact Relationship"                  },
   {"type":"ST",  "desc":"Job Title"                             },
   {"type":"JCC", "desc":"Job Code/class"                        },
   {"type":"XON", "desc":"Guarantor Employer's Organization Name" ,"repeats":true},
   {"type":"IS",  "desc":"Handicap"                              },
   {"type":"IS",  "desc":"Job Status"                            },
   {"type":"FC",  "desc":"Guarantor Financial Class"             },
   {"type":"CE",  "desc":"Guarantor Race"                         ,"repeats":true},
   {"type":"ST",  "desc":"Guarantor Birth Place"                 },
   {"type":"IS",  "desc":"Vip Indicator"                         }]},
 "GTS":{
  "desc":"General Timing Specification",
  "type":"composite"
 },
 "GUARANTOR_INSURANCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                           "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RPII04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RPII04", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQAI08", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQAI09", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQAI10", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQAI11", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQII01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQII01", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQII02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQII02", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQII03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQII03", "desc":""          ,"req":true ,"repeats":true}]},
 "GUARANTOR_INSURANCE_RQII07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"GT1",                                  "desc":"Guarantor" ,"repeats":true},
   {"type":"INSURANCE_GUARANTOR_INSURANCE_RQII07", "desc":""          ,"req":true ,"repeats":true}]},
 "Group1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CER", "desc":"Certificate Detail" ,"req":true},
   {"type":"ROL", "desc":"Role"               ,"repeats":true}]},
 "Group12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"     ,"req":true}]},
 "Group2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",           "desc":"Observation Request" ,"req":true},
   {"type":"NTE",           "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group3_Group2", "desc":""                    ,"repeats":true}]},
 "Group3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group3_Group2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group3_MDMT04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group3_MDMT06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group3_MDMT08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group3_MDMT10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group3_OULR23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",              "desc":"Specimen"           ,"req":true},
   {"type":"OBX",              "desc":"Observation/Result" ,"repeats":true},
   {"type":"CONTAINER_Group3", "desc":""                   ,"req":true ,"repeats":true}]},
 "Group3_REFI12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_REFI13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_RPAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_RPAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "Group3_RRII12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                          "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group3", "desc":""          }]},
 "Group3_RRII13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                                 "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group3_RRII13", "desc":""          }]},
 "Group3_RRII14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                                 "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group3_RRII14", "desc":""          }]},
 "Group3_RRII15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                                 "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group3_RRII15", "desc":""          }]},
 "Group4":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                          "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group4", "desc":""          }]},
 "Group4_REFI13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                                 "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group4_REFI13", "desc":""          }]},
 "Group4_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                                 "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group4_REFI14", "desc":""          }]},
 "Group4_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                                 "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_CONTACT_Group4_REFI15", "desc":""          }]},
 "Group5":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                  "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_Group5", "desc":""          }]},
 "Group5_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                         "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_Group5_RQAI09", "desc":""          }]},
 "Group5_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                         "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_Group5_RQAI10", "desc":""          }]},
 "Group5_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                         "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_Group5_RQAI11", "desc":""          }]},
 "Group5_RRII12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",           "desc":"Observation Request" ,"req":true},
   {"type":"NTE",           "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group6_Group5", "desc":""                    ,"repeats":true}]},
 "Group5_RRII13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group6_Group5_RRII13", "desc":""                    ,"repeats":true}]},
 "Group5_RRII14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group6_Group5_RRII14", "desc":""                    ,"repeats":true}]},
 "Group5_RRII15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group6_Group5_RRII15", "desc":""                    ,"repeats":true}]},
 "Group6":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request"                          ,"req":true},
   {"type":"RQD", "desc":"Requisition Detail"                           ,"req":true},
   {"type":"RQ1", "desc":"Requisition Detail - Additional Information"  ,"req":true},
   {"type":"RXO", "desc":"Pharmacy/Treatment Order"                     ,"req":true},
   {"type":"ODS", "desc":"Dietary Orders, Supplements, And Preferences" ,"req":true},
   {"type":"ODT", "desc":"Diet Tray Instructions"                       ,"req":true}]},
 "Group6_Group5":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group6_Group5_RRII13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group6_Group5_RRII14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group6_Group5_RRII15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group6_REFI12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",    "desc":"Observation Request" ,"req":true},
   {"type":"NTE",    "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7", "desc":""                    ,"repeats":true}]},
 "Group6_REFI13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",           "desc":"Observation Request" ,"req":true},
   {"type":"NTE",           "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7_Group6", "desc":""                    ,"repeats":true}]},
 "Group6_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7_Group6_REFI14", "desc":""                    ,"repeats":true}]},
 "Group6_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7_Group6_REFI15", "desc":""                    ,"repeats":true}]},
 "Group6_RPAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7_Group6_RPAI08", "desc":""                    ,"repeats":true}]},
 "Group6_RPAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7_Group6_RPAI09", "desc":""                    ,"repeats":true}]},
 "Group6_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7_Group6_RPAI10", "desc":""                    ,"repeats":true}]},
 "Group6_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group7_Group6_RPAI11", "desc":""                    ,"repeats":true}]},
 "Group7":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_Group6":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_Group6_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_Group6_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_Group6_RPAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_Group6_RPAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_Group6_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_Group6_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group7_RQAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",    "desc":"Observation Request" ,"req":true},
   {"type":"NTE",    "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group8", "desc":""                    ,"repeats":true}]},
 "Group7_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",           "desc":"Observation Request" ,"req":true},
   {"type":"NTE",           "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group8_Group7", "desc":""                    ,"repeats":true}]},
 "Group7_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group8_Group7_RQAI10", "desc":""                    ,"repeats":true}]},
 "Group7_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                  "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"Group8_Group7_RQAI11", "desc":""                    ,"repeats":true}]},
 "Group8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group8_Group7":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group8_Group7_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "Group8_Group7_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "HD":{
  "desc":"Hierarchic Designator",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Namespace ID"     },
   {"type":"ST", "desc":"Universal ID"     },
   {"type":"ID", "desc":"Universal ID Type"}]},
 "Hxx":{
  "desc":"Any Hl7 Segment",
  "type":"segment"
 },
 "IAM":{
  "desc":"Patient Adverse Reaction Information",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Iam"                             ,"req":true},
   {"type":"CE",  "desc":"Allergen Type Code"                      },
   {"type":"CE",  "desc":"Allergen Code/mnemonic/description"       ,"req":true},
   {"type":"CE",  "desc":"Allergy Severity Code"                   },
   {"type":"ST",  "desc":"Allergy Reaction Code"                    ,"repeats":true},
   {"type":"CNE", "desc":"Allergy Action Code"                      ,"req":true},
   {"type":"EI",  "desc":"Allergy Unique Identifier"               },
   {"type":"ST",  "desc":"Action Reason"                           },
   {"type":"CE",  "desc":"Sensitivity To Causative Agent Code"     },
   {"type":"CE",  "desc":"Allergen Group Code/mnemonic/description"},
   {"type":"DT",  "desc":"Onset Date"                              },
   {"type":"ST",  "desc":"Onset Date Text"                         },
   {"type":"TS",  "desc":"Reported Date/time"                      },
   {"type":"XPN", "desc":"Reported By"                             },
   {"type":"CE",  "desc":"Relationship To Patient Code"            },
   {"type":"CE",  "desc":"Alert Device Code"                       },
   {"type":"CE",  "desc":"Allergy Clinical Status Code"            },
   {"type":"XCN", "desc":"Statused By Person"                      },
   {"type":"XON", "desc":"Statused By Organization"                },
   {"type":"TS",  "desc":"Statused At Date/time"                   }]},
 "ICD":{
  "desc":"Insurance Certification Definition",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Certification Patient Type"      },
   {"type":"ID", "desc":"Certification Required"          },
   {"type":"TS", "desc":"Date/time Certification Required"}]},
 "ID":{
  "desc":"String Data",
  "type":"composite"
 },
 "IIM":{
  "desc":"Inventory Item Master",
  "type":"segment",
  "children":[
   {"type":"CWE", "desc":"Primary Key Value - Iim"          ,"req":true},
   {"type":"CWE", "desc":"Service Item Code"                ,"req":true},
   {"type":"ST",  "desc":"Inventory Lot Number"            },
   {"type":"TS",  "desc":"Inventory Expiration Date"       },
   {"type":"CWE", "desc":"Inventory Manufacturer Name"     },
   {"type":"CWE", "desc":"Inventory Location"              },
   {"type":"TS",  "desc":"Inventory Received Date"         },
   {"type":"NM",  "desc":"Inventory Received Quantity"     },
   {"type":"CWE", "desc":"Inventory Received Quantity Unit"},
   {"type":"MO",  "desc":"Inventory Received Item Cost"    },
   {"type":"TS",  "desc":"Inventory On Hand Date"          },
   {"type":"NM",  "desc":"Inventory On Hand Quantity"      },
   {"type":"CWE", "desc":"Inventory On Hand Quantity Unit" },
   {"type":"CE",  "desc":"Procedure Code"                  },
   {"type":"CE",  "desc":"Procedure Code Modifier"          ,"repeats":true}]},
 "IN1":{
  "desc":"Insurance",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - In1"                      ,"req":true},
   {"type":"CE",  "desc":"Insurance Plan ID"                 ,"req":true},
   {"type":"CX",  "desc":"Insurance Company ID"              ,"req":true ,"repeats":true},
   {"type":"XON", "desc":"Insurance Company Name"            ,"repeats":true},
   {"type":"XAD", "desc":"Insurance Company Address"         ,"repeats":true},
   {"type":"XPN", "desc":"Insurance Co Contact Person"       ,"repeats":true},
   {"type":"XTN", "desc":"Insurance Co Phone Number"         ,"repeats":true},
   {"type":"ST",  "desc":"Group Number"                     },
   {"type":"XON", "desc":"Group Name"                        ,"repeats":true},
   {"type":"CX",  "desc":"Insured's Group Emp ID"            ,"repeats":true},
   {"type":"XON", "desc":"Insured's Group Emp Name"          ,"repeats":true},
   {"type":"DT",  "desc":"Plan Effective Date"              },
   {"type":"DT",  "desc":"Plan Expiration Date"             },
   {"type":"AUI", "desc":"Authorization Information"        },
   {"type":"IS",  "desc":"Plan Type"                        },
   {"type":"XPN", "desc":"Name Of Insured"                   ,"repeats":true},
   {"type":"CE",  "desc":"Insured's Relationship To Patient"},
   {"type":"TS",  "desc":"Insured's Date Of Birth"          },
   {"type":"XAD", "desc":"Insured's Address"                 ,"repeats":true},
   {"type":"IS",  "desc":"Assignment Of Benefits"           },
   {"type":"IS",  "desc":"Coordination Of Benefits"         },
   {"type":"ST",  "desc":"Coord Of Ben. Priority"           },
   {"type":"ID",  "desc":"Notice Of Admission Flag"         },
   {"type":"DT",  "desc":"Notice Of Admission Date"         },
   {"type":"ID",  "desc":"Report Of Eligibility Flag"       },
   {"type":"DT",  "desc":"Report Of Eligibility Date"       },
   {"type":"IS",  "desc":"Release Information Code"         },
   {"type":"ST",  "desc":"Pre-admit Cert (pac)"             },
   {"type":"TS",  "desc":"Verification Date/time"           },
   {"type":"XCN", "desc":"Verification By"                   ,"repeats":true},
   {"type":"IS",  "desc":"Type Of Agreement Code"           },
   {"type":"IS",  "desc":"Billing Status"                   },
   {"type":"NM",  "desc":"Lifetime Reserve Days"            },
   {"type":"NM",  "desc":"Delay Before L.r. Day"            },
   {"type":"IS",  "desc":"Company Plan Code"                },
   {"type":"ST",  "desc":"Policy Number"                    },
   {"type":"CP",  "desc":"Policy Deductible"                },
   {"type":"CP",  "desc":"Policy Limit - Amount"            },
   {"type":"NM",  "desc":"Policy Limit - Days"              },
   {"type":"CP",  "desc":"Room Rate - Semi-private"         },
   {"type":"CP",  "desc":"Room Rate - Private"              },
   {"type":"CE",  "desc":"Insured's Employment Status"      },
   {"type":"IS",  "desc":"Insured's Administrative Sex"     },
   {"type":"XAD", "desc":"Insured's Employer's Address"      ,"repeats":true},
   {"type":"ST",  "desc":"Verification Status"              },
   {"type":"IS",  "desc":"Prior Insurance Plan ID"          },
   {"type":"IS",  "desc":"Coverage Type"                    },
   {"type":"IS",  "desc":"Handicap"                         },
   {"type":"CX",  "desc":"Insured's ID Number"               ,"repeats":true},
   {"type":"IS",  "desc":"Signature Code"                   },
   {"type":"DT",  "desc":"Signature Code Date"              },
   {"type":"ST",  "desc":"Insured_s Birth Place"            },
   {"type":"IS",  "desc":"Vip Indicator"                    }]},
 "IN2":{
  "desc":"Insurance Additional Information",
  "type":"segment",
  "children":[
   {"type":"CX",  "desc":"Insured's Employee ID"                     ,"repeats":true},
   {"type":"ST",  "desc":"Insured's Social Security Number"         },
   {"type":"XCN", "desc":"Insured's Employer's Name And ID"          ,"repeats":true},
   {"type":"IS",  "desc":"Employer Information Data"                },
   {"type":"IS",  "desc":"Mail Claim Party"                          ,"repeats":true},
   {"type":"ST",  "desc":"Medicare Health Ins Card Number"          },
   {"type":"XPN", "desc":"Medicaid Case Name"                        ,"repeats":true},
   {"type":"ST",  "desc":"Medicaid Case Number"                     },
   {"type":"XPN", "desc":"Military Sponsor Name"                     ,"repeats":true},
   {"type":"ST",  "desc":"Military ID Number"                       },
   {"type":"CE",  "desc":"Dependent Of Military Recipient"          },
   {"type":"ST",  "desc":"Military Organization"                    },
   {"type":"ST",  "desc":"Military Station"                         },
   {"type":"IS",  "desc":"Military Service"                         },
   {"type":"IS",  "desc":"Military Rank/grade"                      },
   {"type":"IS",  "desc":"Military Status"                          },
   {"type":"DT",  "desc":"Military Retire Date"                     },
   {"type":"ID",  "desc":"Military Non-avail Cert On File"          },
   {"type":"ID",  "desc":"Baby Coverage"                            },
   {"type":"ID",  "desc":"Combine Baby Bill"                        },
   {"type":"ST",  "desc":"Blood Deductible"                         },
   {"type":"XPN", "desc":"Special Coverage Approval Name"            ,"repeats":true},
   {"type":"ST",  "desc":"Special Coverage Approval Title"          },
   {"type":"IS",  "desc":"Non-covered Insurance Code"                ,"repeats":true},
   {"type":"CX",  "desc":"Payor ID"                                  ,"repeats":true},
   {"type":"CX",  "desc":"Payor Subscriber ID"                       ,"repeats":true},
   {"type":"IS",  "desc":"Eligibility Source"                       },
   {"type":"RMC", "desc":"Room Coverage Type/amount"                 ,"repeats":true},
   {"type":"PTA", "desc":"Policy Type/amount"                        ,"repeats":true},
   {"type":"DDI", "desc":"Daily Deductible"                         },
   {"type":"IS",  "desc":"Living Dependency"                        },
   {"type":"IS",  "desc":"Ambulatory Status"                         ,"repeats":true},
   {"type":"CE",  "desc":"Citizenship"                               ,"repeats":true},
   {"type":"CE",  "desc":"Primary Language"                         },
   {"type":"IS",  "desc":"Living Arrangement"                       },
   {"type":"CE",  "desc":"Publicity Code"                           },
   {"type":"ID",  "desc":"Protection Indicator"                     },
   {"type":"IS",  "desc":"Student Indicator"                        },
   {"type":"CE",  "desc":"Religion"                                 },
   {"type":"XPN", "desc":"Mother's Maiden Name"                      ,"repeats":true},
   {"type":"CE",  "desc":"Nationality"                              },
   {"type":"CE",  "desc":"Ethnic Group"                              ,"repeats":true},
   {"type":"CE",  "desc":"Marital Status"                            ,"repeats":true},
   {"type":"DT",  "desc":"Insured's Employment Start Date"          },
   {"type":"DT",  "desc":"Employment Stop Date"                     },
   {"type":"ST",  "desc":"Job Title"                                },
   {"type":"JCC", "desc":"Job Code/class"                           },
   {"type":"IS",  "desc":"Job Status"                               },
   {"type":"XPN", "desc":"Employer Contact Person Name"              ,"repeats":true},
   {"type":"XTN", "desc":"Employer Contact Person Phone Number"      ,"repeats":true},
   {"type":"IS",  "desc":"Employer Contact Reason"                  },
   {"type":"XPN", "desc":"Insured's Contact Person's Name"           ,"repeats":true},
   {"type":"XTN", "desc":"Insured's Contact Person Phone Number"     ,"repeats":true},
   {"type":"IS",  "desc":"Insured's Contact Person Reason"           ,"repeats":true},
   {"type":"DT",  "desc":"Relationship To The Patient Start Date"   },
   {"type":"DT",  "desc":"Relationship To The Patient Stop Date"     ,"repeats":true},
   {"type":"IS",  "desc":"Insurance Co. Contact Reason"             },
   {"type":"XTN", "desc":"Insurance Co Contact Phone Number"        },
   {"type":"IS",  "desc":"Policy Scope"                             },
   {"type":"IS",  "desc":"Policy Source"                            },
   {"type":"CX",  "desc":"Patient Member Number"                    },
   {"type":"CE",  "desc":"Guarantor's Relationship To Insured"      },
   {"type":"XTN", "desc":"Insured's Phone Number - Home"             ,"repeats":true},
   {"type":"XTN", "desc":"Insured's Employer Phone Number"           ,"repeats":true},
   {"type":"CE",  "desc":"Military Handicapped Program"             },
   {"type":"ID",  "desc":"Suspend Flag"                             },
   {"type":"ID",  "desc":"Copay Limit Flag"                         },
   {"type":"ID",  "desc":"Stoploss Limit Flag"                      },
   {"type":"XON", "desc":"Insured Organization Name And ID"          ,"repeats":true},
   {"type":"XON", "desc":"Insured Employer Organization Name And ID" ,"repeats":true},
   {"type":"CE",  "desc":"Race"                                      ,"repeats":true},
   {"type":"CE",  "desc":"Cms Patient_s Relationship To Insured"    }]},
 "IN3":{
  "desc":"Insurance Additional Information, Certification",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - In3"                          ,"req":true},
   {"type":"CX",  "desc":"Certification Number"                 },
   {"type":"XCN", "desc":"Certified By"                          ,"repeats":true},
   {"type":"ID",  "desc":"Certification Required"               },
   {"type":"MOP", "desc":"Penalty"                              },
   {"type":"TS",  "desc":"Certification Date/time"              },
   {"type":"TS",  "desc":"Certification Modify Date/time"       },
   {"type":"XCN", "desc":"Operator"                              ,"repeats":true},
   {"type":"DT",  "desc":"Certification Begin Date"             },
   {"type":"DT",  "desc":"Certification End Date"               },
   {"type":"DTN", "desc":"Days"                                 },
   {"type":"CE",  "desc":"Non-concur Code/description"          },
   {"type":"TS",  "desc":"Non-concur Effective Date/time"       },
   {"type":"XCN", "desc":"Physician Reviewer"                    ,"repeats":true},
   {"type":"ST",  "desc":"Certification Contact"                },
   {"type":"XTN", "desc":"Certification Contact Phone Number"    ,"repeats":true},
   {"type":"CE",  "desc":"Appeal Reason"                        },
   {"type":"CE",  "desc":"Certification Agency"                 },
   {"type":"XTN", "desc":"Certification Agency Phone Number"     ,"repeats":true},
   {"type":"ICD", "desc":"Pre-certification Requirement"         ,"repeats":true},
   {"type":"ST",  "desc":"Case Manager"                         },
   {"type":"DT",  "desc":"Second Opinion Date"                  },
   {"type":"IS",  "desc":"Second Opinion Status"                },
   {"type":"IS",  "desc":"Second Opinion Documentation Received" ,"repeats":true},
   {"type":"XCN", "desc":"Second Opinion Physician"              ,"repeats":true}]},
 "INRU06":{
  "desc":"Automated equipment inventory request",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"   ,"req":true},
   {"type":"SFT", "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU", "desc":"Equipment Detail" ,"req":true},
   {"type":"INV", "desc":"Inventory Detail" ,"req":true ,"repeats":true},
   {"type":"ROL", "desc":"Role"            }]},
 "INSURANCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA28":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_ADTA31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_DFTP03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_DFTP11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_GUARANTOR_INSURANCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RPII04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQII01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQII02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQII03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_GUARANTOR_INSURANCE_RQII07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMDO03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMGO19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMIO23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMLO21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMLO33":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMLO35":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMNO07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMPO09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_OMSO05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_ORMO01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_RDEO11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_PATIENT_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_VISIT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_VISIT_BARP05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification" ,"repeats":true},
   {"type":"ROL", "desc":"Role"                                            ,"repeats":true}]},
 "INSURANCE_VXRV03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INSURANCE_VXUV04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"IN1", "desc":"Insurance"                                       ,"req":true},
   {"type":"IN2", "desc":"Insurance Additional Information"               },
   {"type":"IN3", "desc":"Insurance Additional Information, Certification"}]},
 "INUU05":{
  "desc":"Automated equipment inventory update",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"   ,"req":true},
   {"type":"SFT", "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU", "desc":"Equipment Detail" ,"req":true},
   {"type":"INV", "desc":"Inventory Detail" ,"req":true ,"repeats":true},
   {"type":"ROL", "desc":"Role"            }]},
 "INV":{
  "desc":"Inventory Detail",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Substance Identifier"           ,"req":true},
   {"type":"CE", "desc":"Substance Status"               ,"req":true ,"repeats":true},
   {"type":"CE", "desc":"Substance Type"                },
   {"type":"CE", "desc":"Inventory Container Identifier"},
   {"type":"CE", "desc":"Container Carrier Identifier"  },
   {"type":"CE", "desc":"Position On Carrier"           },
   {"type":"NM", "desc":"Initial Quantity"              },
   {"type":"NM", "desc":"Current Quantity"              },
   {"type":"NM", "desc":"Available Quantity"            },
   {"type":"NM", "desc":"Consumption Quantity"          },
   {"type":"CE", "desc":"Quantity Units"                },
   {"type":"TS", "desc":"Expiration Date/time"          },
   {"type":"TS", "desc":"First Used Date/time"          },
   {"type":"TQ", "desc":"On Board Stability Duration"   },
   {"type":"CE", "desc":"Test/fluid Identifier(s)"       ,"repeats":true},
   {"type":"ST", "desc":"Manufacturer Lot Number"       },
   {"type":"CE", "desc":"Manufacturer Identifier"       },
   {"type":"CE", "desc":"Supplier Identifier"           },
   {"type":"CQ", "desc":"On Board Stability Time"       },
   {"type":"CQ", "desc":"Target Value"                  }]},
 "IPC":{
  "desc":"Imaging Procedure Control Segment",
  "type":"segment",
  "children":[
   {"type":"EI", "desc":"Accession Identifier"              ,"req":true},
   {"type":"EI", "desc":"Requested Procedure ID"            ,"req":true},
   {"type":"EI", "desc":"Study Instance Uid"                ,"req":true},
   {"type":"EI", "desc":"Scheduled Procedure Step ID"       ,"req":true},
   {"type":"CE", "desc":"Modality"                         },
   {"type":"CE", "desc":"Protocol Code"                     ,"repeats":true},
   {"type":"EI", "desc":"Scheduled Station Name"           },
   {"type":"CE", "desc":"Scheduled Procedure Step Location" ,"repeats":true},
   {"type":"ST", "desc":"Scheduled Ae Title"               }]},
 "IS":{
  "desc":"String Data",
  "type":"composite"
 },
 "ISD":{
  "desc":"Interaction Status Detail",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Reference Interaction Number (unique Identifier)" ,"req":true},
   {"type":"CE", "desc":"Interaction Type Identifier"                     },
   {"type":"CE", "desc":"Interaction Active State"                         ,"req":true}]},
 "JCC":{
  "desc":"Job Code/Class",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Job Code"            },
   {"type":"IS", "desc":"Job Class"           },
   {"type":"TX", "desc":"Job Description Text"}]},
 "LA1":{
  "desc":"Location With Address Variation 1",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Point Of Care"        },
   {"type":"IS", "desc":"Room"                 },
   {"type":"IS", "desc":"Bed"                  },
   {"type":"HD", "desc":"Facility"             },
   {"type":"IS", "desc":"Location Status"      },
   {"type":"IS", "desc":"Patient Location Type"},
   {"type":"IS", "desc":"Building"             },
   {"type":"IS", "desc":"Floor"                },
   {"type":"AD", "desc":"Address"              }]},
 "LA2":{
  "desc":"Location With Address Variation 2",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Point Of Care"               },
   {"type":"IS", "desc":"Room"                        },
   {"type":"IS", "desc":"Bed"                         },
   {"type":"HD", "desc":"Facility"                    },
   {"type":"IS", "desc":"Location Status"             },
   {"type":"IS", "desc":"Patient Location Type"       },
   {"type":"IS", "desc":"Building"                    },
   {"type":"IS", "desc":"Floor"                       },
   {"type":"ST", "desc":"Street Address"              },
   {"type":"ST", "desc":"Other Designation"           },
   {"type":"ST", "desc":"City"                        },
   {"type":"ST", "desc":"State Or Province"           },
   {"type":"ST", "desc":"Zip Or Postal Code"          },
   {"type":"ID", "desc":"Country"                     },
   {"type":"ID", "desc":"Address Type"                },
   {"type":"ST", "desc":"Other Geographic Designation"}]},
 "LAN":{
  "desc":"Language Detail",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID _ Lan"              ,"req":true},
   {"type":"CE", "desc":"Language Code"             ,"req":true},
   {"type":"CE", "desc":"Language Ability Code"     ,"repeats":true},
   {"type":"CE", "desc":"Language Proficiency Code"}]},
 "LCC":{
  "desc":"Location Charge Code",
  "type":"segment",
  "children":[
   {"type":"PL", "desc":"Primary Key Value - Lcc" ,"req":true},
   {"type":"CE", "desc":"Location Department"     ,"req":true},
   {"type":"CE", "desc":"Accommodation Type"      ,"repeats":true},
   {"type":"CE", "desc":"Charge Code"             ,"req":true ,"repeats":true}]},
 "LCH":{
  "desc":"Location Characteristic",
  "type":"segment",
  "children":[
   {"type":"PL", "desc":"Primary Key Value - Lch"           ,"req":true},
   {"type":"ID", "desc":"Segment Action Code"              },
   {"type":"EI", "desc":"Segment Unique Key"               },
   {"type":"CE", "desc":"Location Characteristic ID"        ,"req":true},
   {"type":"CE", "desc":"Location Characteristic Value-lch" ,"req":true}]},
 "LDP":{
  "desc":"Location Department",
  "type":"segment",
  "children":[
   {"type":"PL",  "desc":"Primary Key Value - Ldp" ,"req":true},
   {"type":"CE",  "desc":"Location Department"     ,"req":true},
   {"type":"IS",  "desc":"Location Service"        ,"repeats":true},
   {"type":"CE",  "desc":"Specialty Type"          ,"repeats":true},
   {"type":"IS",  "desc":"Valid Patient Classes"   ,"repeats":true},
   {"type":"ID",  "desc":"Active/inactive Flag"   },
   {"type":"TS",  "desc":"Activation Date Ldp"    },
   {"type":"TS",  "desc":"Inactivation Date - Ldp"},
   {"type":"ST",  "desc":"Inactivated Reason"     },
   {"type":"VH",  "desc":"Visiting Hours"          ,"repeats":true},
   {"type":"XTN", "desc":"Contact Phone"          },
   {"type":"CE",  "desc":"Location Cost Center"   }]},
 "LOC":{
  "desc":"Location Identification",
  "type":"segment",
  "children":[
   {"type":"PL",  "desc":"Primary Key Value - Loc" ,"req":true},
   {"type":"ST",  "desc":"Location Description"   },
   {"type":"IS",  "desc":"Location Type - Loc"     ,"req":true ,"repeats":true},
   {"type":"XON", "desc":"Organization Name - Loc" ,"repeats":true},
   {"type":"XAD", "desc":"Location Address"        ,"repeats":true},
   {"type":"XTN", "desc":"Location Phone"          ,"repeats":true},
   {"type":"CE",  "desc":"License Number"          ,"repeats":true},
   {"type":"IS",  "desc":"Location Equipment"      ,"repeats":true},
   {"type":"IS",  "desc":"Location Service Code"  }]},
 "LOCATION_RESOURCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    }]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SIUS26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LOCATION_RESOURCE_RESOURCES_SRMS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIL", "desc":"Appointment Information - Location Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                    },
   {"type":"NTE", "desc":"Notes And Comments"                          ,"repeats":true}]},
 "LRL":{
  "desc":"Location Relationship",
  "type":"segment",
  "children":[
   {"type":"PL",  "desc":"Primary Key Value - Lrl"                    ,"req":true},
   {"type":"ID",  "desc":"Segment Action Code"                       },
   {"type":"EI",  "desc":"Segment Unique Key"                        },
   {"type":"CE",  "desc":"Location Relationship ID"                   ,"req":true},
   {"type":"XON", "desc":"Organizational Location Relationship Value" ,"repeats":true},
   {"type":"PL",  "desc":"Patient Location Relationship Value"       }]},
 "LSUU12":{
  "desc":"Automated equipment log/service update",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"        ,"req":true},
   {"type":"SFT", "desc":"Software Segment"      ,"repeats":true},
   {"type":"EQU", "desc":"Equipment Detail"      ,"req":true},
   {"type":"EQP", "desc":"Equipment/Log Service" ,"req":true ,"repeats":true},
   {"type":"ROL", "desc":"Role"                 }]},
 "LSUU13":{
  "desc":"Automated equipment log/service request",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"        ,"req":true},
   {"type":"SFT", "desc":"Software Segment"      ,"repeats":true},
   {"type":"EQU", "desc":"Equipment Detail"      ,"req":true},
   {"type":"EQP", "desc":"Equipment/Log Service" ,"req":true ,"repeats":true},
   {"type":"ROL", "desc":"Role"                 }]},
 "MDMT01":{
  "desc":"Original document notification",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT01", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true}]},
 "MDMT02":{
  "desc":"Original document notification and content",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT02", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true},
   {"type":"Group3",              "desc":""                              ,"req":true ,"repeats":true}]},
 "MDMT03":{
  "desc":"Document status change notification",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT03", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true}]},
 "MDMT04":{
  "desc":"Document status change notification and content",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT04", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true},
   {"type":"Group3_MDMT04",       "desc":""                              ,"req":true ,"repeats":true}]},
 "MDMT05":{
  "desc":"Document addendum notification",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT05", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true}]},
 "MDMT06":{
  "desc":"Document addendum notification and content",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT06", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true},
   {"type":"Group3_MDMT06",       "desc":""                              ,"req":true ,"repeats":true}]},
 "MDMT07":{
  "desc":"Document edit notification",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT07", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true}]},
 "MDMT08":{
  "desc":"Document edit notification and content",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT08", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true},
   {"type":"Group3_MDMT08",       "desc":""                              ,"req":true ,"repeats":true}]},
 "MDMT09":{
  "desc":"Document replacement notification",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT09", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true}]},
 "MDMT10":{
  "desc":"Document replacement notification and content",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT10", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true},
   {"type":"Group3_MDMT10",       "desc":""                              ,"req":true ,"repeats":true}]},
 "MDMT11":{
  "desc":"Document cancel notification",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"              ,"repeats":true},
   {"type":"EVN",                 "desc":"Event Type"                    ,"req":true},
   {"type":"PID",                 "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1",                 "desc":"Patient Visit"                 ,"req":true},
   {"type":"COMMON_ORDER_MDMT11", "desc":""                              ,"repeats":true},
   {"type":"TXA",                 "desc":"Transcription Document Header" ,"req":true}]},
 "MERGE_INFO":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MRG", "desc":"Merge Patient Information" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"             ,"req":true}]},
 "MF":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"  }]},
 "MFA":{
  "desc":"Master File Acknowledgment",
  "type":"segment",
  "children":[
   {"type":"ID",     "desc":"Record-level Event Code"       ,"req":true},
   {"type":"ST",     "desc":"Mfn Control ID"               },
   {"type":"TS",     "desc":"Event Completion Date/time"   },
   {"type":"CE",     "desc":"Mfn Record Level Error Return" ,"req":true},
   {"type":"Varies", "desc":"Primary Key Value - Mfa"       ,"req":true ,"repeats":true},
   {"type":"ID",     "desc":"Primary Key Value Type - Mfa"  ,"req":true ,"repeats":true}]},
 "MFE":{
  "desc":"Master File Entry",
  "type":"segment",
  "children":[
   {"type":"ID",     "desc":"Record-level Event Code" ,"req":true},
   {"type":"ST",     "desc":"Mfn Control ID"         },
   {"type":"TS",     "desc":"Effective Date/time"    },
   {"type":"Varies", "desc":"Primary Key Value - Mfe" ,"req":true ,"repeats":true},
   {"type":"ID",     "desc":"Primary Key Value Type"  ,"req":true ,"repeats":true}]},
 "MFI":{
  "desc":"Master File Identification",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Master File Identifier"             ,"req":true},
   {"type":"HD", "desc":"Master File Application Identifier"},
   {"type":"ID", "desc":"File-level Event Code"              ,"req":true},
   {"type":"TS", "desc":"Entered Date/time"                 },
   {"type":"TS", "desc":"Effective Date/time"               },
   {"type":"ID", "desc":"Response Level Code"                ,"req":true}]},
 "MFKM01":{
  "desc":"Master file - not otherwise specified (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM02":{
  "desc":"Master file - staff practitioner - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM03":{
  "desc":"Master file - test/observation (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM04":{
  "desc":"Master files charge description - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM05":{
  "desc":"Patient location master file - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM06":{
  "desc":"Clinical study with phases and schedules master file - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM07":{
  "desc":"Clinical study without phases but with schedules master file - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM08":{
  "desc":"Test/observation (numeric) master file - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM09":{
  "desc":"Test/observation (categorical) master file - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM10":{
  "desc":"Test /observation batteries master file - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM11":{
  "desc":"Test/calculated observations master file - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM12":{
  "desc":"Master file notification message - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM13":{
  "desc":"General - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM14":{
  "desc":"Master file notification - site defined - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFKM15":{
  "desc":"Inventory item master file notification - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                      ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFA", "desc":"Master File Acknowledgment" ,"repeats":true}]},
 "MFNM01":{
  "desc":"Master file not otherwise specified (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MF",  "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM02":{
  "desc":"Master file - staff/practitioner",
  "type":"message",
  "children":[
   {"type":"MSH",      "desc":"Message Header"             ,"req":true},
   {"type":"SFT",      "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",      "desc":"Master File Identification" ,"req":true},
   {"type":"MF_STAFF", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM03":{
  "desc":"Master file - test/observation (for backward compatibility only)",
  "type":"message",
  "children":[
   {"type":"MSH",     "desc":"Message Header"             ,"req":true},
   {"type":"SFT",     "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",     "desc":"Master File Identification" ,"req":true},
   {"type":"MF_TEST", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM04":{
  "desc":"Master files charge description",
  "type":"message",
  "children":[
   {"type":"MSH",    "desc":"Message Header"             ,"req":true},
   {"type":"SFT",    "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",    "desc":"Master File Identification" ,"req":true},
   {"type":"MF_CDM", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM05":{
  "desc":"Patient location master file",
  "type":"message",
  "children":[
   {"type":"MSH",         "desc":"Message Header"             ,"req":true},
   {"type":"SFT",         "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",         "desc":"Master File Identification" ,"req":true},
   {"type":"MF_LOCATION", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM06":{
  "desc":"Clinical study with phases and schedules master file",
  "type":"message",
  "children":[
   {"type":"MSH",           "desc":"Message Header"             ,"req":true},
   {"type":"SFT",           "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",           "desc":"Master File Identification" ,"req":true},
   {"type":"MF_CLIN_STUDY", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM07":{
  "desc":"Clinical study without phases but with schedules master file",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",                 "desc":"Master File Identification" ,"req":true},
   {"type":"MF_CLIN_STUDY_SCHED", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM08":{
  "desc":"Test/observation (numeric) master file",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"             ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",             "desc":"Master File Identification" ,"req":true},
   {"type":"MF_TEST_NUMERIC", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM09":{
  "desc":"Test/observation (categorical) master file",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",                 "desc":"Master File Identification" ,"req":true},
   {"type":"MF_TEST_CATEGORICAL", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM10":{
  "desc":"Test/observation (categorical) master file",
  "type":"message",
  "children":[
   {"type":"MSH",               "desc":"Message Header"             ,"req":true},
   {"type":"SFT",               "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",               "desc":"Master File Identification" ,"req":true},
   {"type":"MF_TEST_BATTERIES", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM11":{
  "desc":"Test/calculated observations master file",
  "type":"message",
  "children":[
   {"type":"MSH",                "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",                "desc":"Master File Identification" ,"req":true},
   {"type":"MF_TEST_CALCULATED", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM12":{
  "desc":"Master file notification message",
  "type":"message",
  "children":[
   {"type":"MSH",               "desc":"Message Header"             ,"req":true},
   {"type":"SFT",               "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",               "desc":"Master File Identification" ,"req":true},
   {"type":"MF_OBS_ATTRIBUTES", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM13":{
  "desc":"Master file notification - general",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI", "desc":"Master File Identification" ,"req":true},
   {"type":"MFE", "desc":"Master File Entry"          ,"req":true ,"repeats":true}]},
 "MFNM14":{
  "desc":"Master file notification - site defined",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"             ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",             "desc":"Master File Identification" ,"req":true},
   {"type":"MF_SITE_DEFINED", "desc":""                           ,"req":true ,"repeats":true}]},
 "MFNM15":{
  "desc":"Inventory item master file notification",
  "type":"message",
  "children":[
   {"type":"MSH",         "desc":"Message Header"             ,"req":true},
   {"type":"SFT",         "desc":"Software Segment"           ,"repeats":true},
   {"type":"MFI",         "desc":"Master File Identification" ,"req":true},
   {"type":"MF_INV_ITEM", "desc":""                           ,"req":true ,"repeats":true}]},
 "MF_CDM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry"         ,"req":true},
   {"type":"CDM", "desc":"Charge Description Master" ,"req":true},
   {"type":"PRC", "desc":"Pricing"                   ,"repeats":true}]},
 "MF_CLIN_STUDY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE",                   "desc":"Master File Entry"     ,"req":true},
   {"type":"CM0",                   "desc":"Clinical Study Master" ,"req":true},
   {"type":"MF_PHASE_SCHED_DETAIL", "desc":""                      ,"repeats":true}]},
 "MF_CLIN_STUDY_SCHED":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry"              ,"req":true},
   {"type":"CM0", "desc":"Clinical Study Master"          ,"req":true},
   {"type":"CM2", "desc":"Clinical Study Schedule Master" ,"repeats":true}]},
 "MF_INV_ITEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry"     ,"req":true},
   {"type":"IIM", "desc":"Inventory Item Master" ,"req":true}]},
 "MF_LOCATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE",         "desc":"Master File Entry"       ,"req":true},
   {"type":"LOC",         "desc":"Location Identification" ,"req":true},
   {"type":"LCH",         "desc":"Location Characteristic" ,"repeats":true},
   {"type":"LRL",         "desc":"Location Relationship"   ,"repeats":true},
   {"type":"MF_LOC_DEPT", "desc":""                        ,"req":true ,"repeats":true}]},
 "MF_LOC_DEPT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"LDP", "desc":"Location Department"     ,"req":true},
   {"type":"LCH", "desc":"Location Characteristic" ,"repeats":true},
   {"type":"LCC", "desc":"Location Charge Code"    ,"repeats":true}]},
 "MF_OBS_ATTRIBUTES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry"           ,"req":true},
   {"type":"OM1", "desc":"General Segment"             ,"req":true},
   {"type":"OM7", "desc":"Additional Basic Attributes"}]},
 "MF_PHASE_SCHED_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CM1", "desc":"Clinical Study Phase Master"    ,"req":true},
   {"type":"CM2", "desc":"Clinical Study Schedule Master" ,"repeats":true}]},
 "MF_SITE_DEFINED":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"   ,"req":true}]},
 "MF_STAFF":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry"              ,"req":true},
   {"type":"STF", "desc":"Staff Identification"           ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail"            ,"repeats":true},
   {"type":"ORG", "desc":"Practitioner Organization Unit" ,"repeats":true},
   {"type":"AFF", "desc":"Professional Affiliation"       ,"repeats":true},
   {"type":"LAN", "desc":"Language Detail"                ,"repeats":true},
   {"type":"EDU", "desc":"Educational Detail"             ,"repeats":true},
   {"type":"CER", "desc":"Certificate Detail"             ,"repeats":true},
   {"type":"NTE", "desc":"Notes And Comments"             ,"repeats":true}]},
 "MF_TEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry" ,"req":true},
   {"type":"OM1", "desc":"General Segment"   ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"   ,"req":true}]},
 "MF_TEST_BATTERIES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE",                 "desc":"Master File Entry" ,"req":true},
   {"type":"OM1",                 "desc":"General Segment"   ,"req":true},
   {"type":"MF_TEST_BATT_DETAIL", "desc":""                 }]},
 "MF_TEST_BATT_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OM5", "desc":"Observation Batteries (sets)"        ,"req":true},
   {"type":"OM4", "desc":"Observations That Require Specimens" ,"repeats":true}]},
 "MF_TEST_CALCULATED":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE",                 "desc":"Master File Entry" ,"req":true},
   {"type":"OM1",                 "desc":"General Segment"   ,"req":true},
   {"type":"MF_TEST_CALC_DETAIL", "desc":""                 }]},
 "MF_TEST_CALC_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OM6", "desc":"Observations That Are Calculated From Other Observations" ,"req":true},
   {"type":"OM2", "desc":"Numeric Observation"                                      ,"req":true}]},
 "MF_TEST_CATEGORICAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE",                "desc":"Master File Entry" ,"req":true},
   {"type":"OM1",                "desc":"General Segment"   ,"req":true},
   {"type":"MF_TEST_CAT_DETAIL", "desc":""                 }]},
 "MF_TEST_CAT_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OM3", "desc":"Categorical Service/test/observation" ,"req":true},
   {"type":"OM4", "desc":"Observations That Require Specimens"  ,"repeats":true}]},
 "MF_TEST_NUMERIC":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"MFE", "desc":"Master File Entry"                    ,"req":true},
   {"type":"OM1", "desc":"General Segment"                      ,"req":true},
   {"type":"OM2", "desc":"Numeric Observation"                 },
   {"type":"OM3", "desc":"Categorical Service/test/observation"},
   {"type":"OM4", "desc":"Observations That Require Specimens" }]},
 "MO":{
  "desc":"Money",
  "type":"composite",
  "children":[
   {"type":"NM", "desc":"Quantity"    },
   {"type":"ID", "desc":"Denomination"}]},
 "MOC":{
  "desc":"Money And Code",
  "type":"composite",
  "children":[
   {"type":"MO", "desc":"Monetary Amount"},
   {"type":"CE", "desc":"Charge Code"    }]},
 "MOP":{
  "desc":"Money Or Percentage",
  "type":"composite",
  "children":[
   {"type":"ID", "desc":"Money Or Percentage Indicator"},
   {"type":"NM", "desc":"Money Or Percentage Quantity" },
   {"type":"ID", "desc":"Currency Denomination"        }]},
 "MRG":{
  "desc":"Merge Patient Information",
  "type":"segment",
  "children":[
   {"type":"CX",  "desc":"Prior Patient Identifier List" ,"req":true ,"repeats":true},
   {"type":"CX",  "desc":"Prior Alternate Patient ID"    ,"repeats":true},
   {"type":"CX",  "desc":"Prior Patient Account Number" },
   {"type":"CX",  "desc":"Prior Patient ID"             },
   {"type":"CX",  "desc":"Prior Visit Number"           },
   {"type":"CX",  "desc":"Prior Alternate Visit ID"     },
   {"type":"XPN", "desc":"Prior Patient Name"            ,"repeats":true}]},
 "MSA":{
  "desc":"Message Acknowledgment",
  "type":"segment",
  "children":[
   {"type":"ID", "desc":"Acknowledgment Code"         ,"req":true},
   {"type":"ST", "desc":"Message Control ID"          ,"req":true},
   {"type":"ST", "desc":"Text Message"               },
   {"type":"NM", "desc":"Expected Sequence Number"   },
   {"type":"ID", "desc":"Delayed Acknowledgment Type"},
   {"type":"CE", "desc":"Error Condition"            }]},
 "MSG":{
  "desc":"Message Type",
  "type":"composite",
  "children":[
   {"type":"ID", "desc":"Message Code"     },
   {"type":"ID", "desc":"Trigger Event"    },
   {"type":"ID", "desc":"Message Structure"}]},
 "MSH":{
  "desc":"Message Header",
  "type":"segment",
  "children":[
   {"type":"ST",  "desc":"Field Separator"                         ,"req":true},
   {"type":"ST",  "desc":"Encoding Characters"                     ,"req":true},
   {"type":"HD",  "desc":"Sending Application"                    },
   {"type":"HD",  "desc":"Sending Facility"                       },
   {"type":"HD",  "desc":"Receiving Application"                  },
   {"type":"HD",  "desc":"Receiving Facility"                     },
   {"type":"TS",  "desc":"Date/time Of Message"                    ,"req":true},
   {"type":"ST",  "desc":"Security"                               },
   {"type":"MSG", "desc":"Message Type"                            ,"req":true},
   {"type":"ST",  "desc":"Message Control ID"                      ,"req":true},
   {"type":"PT",  "desc":"Processing ID"                           ,"req":true},
   {"type":"VID", "desc":"Version ID"                              ,"req":true},
   {"type":"NM",  "desc":"Sequence Number"                        },
   {"type":"ST",  "desc":"Continuation Pointer"                   },
   {"type":"ID",  "desc":"Accept Acknowledgment Type"             },
   {"type":"ID",  "desc":"Application Acknowledgment Type"        },
   {"type":"ID",  "desc":"Country Code"                           },
   {"type":"ID",  "desc":"Character Set"                           ,"repeats":true},
   {"type":"CE",  "desc":"Principal Language Of Message"          },
   {"type":"ID",  "desc":"Alternate Character Set Handling Scheme"},
   {"type":"EI",  "desc":"Message Profile Identifier"              ,"repeats":true}]},
 "NA":{
  "desc":"Numeric Array",
  "type":"composite",
  "children":[
   {"type":"NM", "desc":"Value1"},
   {"type":"NM", "desc":"Value2"},
   {"type":"NM", "desc":"Value3"},
   {"type":"NM", "desc":"Value4"}]},
 "NCK":{
  "desc":"System Clock",
  "type":"segment",
  "children":[
   {"type":"TS", "desc":"System Date/time" ,"req":true}]},
 "NDL":{
  "desc":"Name With Date And Location",
  "type":"composite",
  "children":[
   {"type":"CNN", "desc":"Name"                 },
   {"type":"TS",  "desc":"Start Date/time"      },
   {"type":"TS",  "desc":"End Date/time"        },
   {"type":"IS",  "desc":"Point Of Care"        },
   {"type":"IS",  "desc":"Room"                 },
   {"type":"IS",  "desc":"Bed"                  },
   {"type":"HD",  "desc":"Facility"             },
   {"type":"IS",  "desc":"Location Status"      },
   {"type":"IS",  "desc":"Patient Location Type"},
   {"type":"IS",  "desc":"Building"             },
   {"type":"IS",  "desc":"Floor"                }]},
 "NDS":{
  "desc":"Notification Detail",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Notification Reference Number" ,"req":true},
   {"type":"TS", "desc":"Notification Date/time"        ,"req":true},
   {"type":"CE", "desc":"Notification Alert Severity"   ,"req":true},
   {"type":"CE", "desc":"Notification Code"             ,"req":true}]},
 "NK1":{
  "desc":"Next Of Kin / Associated Parties",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Nk1"                                     ,"req":true},
   {"type":"XPN", "desc":"Name"                                             ,"repeats":true},
   {"type":"CE",  "desc":"Relationship"                                    },
   {"type":"XAD", "desc":"Address"                                          ,"repeats":true},
   {"type":"XTN", "desc":"Phone Number"                                     ,"repeats":true},
   {"type":"XTN", "desc":"Business Phone Number"                            ,"repeats":true},
   {"type":"CE",  "desc":"Contact Role"                                    },
   {"type":"DT",  "desc":"Start Date"                                      },
   {"type":"DT",  "desc":"End Date"                                        },
   {"type":"ST",  "desc":"Next Of Kin / Associated Parties Job Title"      },
   {"type":"JCC", "desc":"Next Of Kin / Associated Parties Job Code/class" },
   {"type":"CX",  "desc":"Next Of Kin / Associated Parties Employee Number"},
   {"type":"XON", "desc":"Organization Name - Nk1"                          ,"repeats":true},
   {"type":"CE",  "desc":"Marital Status"                                  },
   {"type":"IS",  "desc":"Administrative Sex"                              },
   {"type":"TS",  "desc":"Date/time Of Birth"                              },
   {"type":"IS",  "desc":"Living Dependency"                                ,"repeats":true},
   {"type":"IS",  "desc":"Ambulatory Status"                                ,"repeats":true},
   {"type":"CE",  "desc":"Citizenship"                                      ,"repeats":true},
   {"type":"CE",  "desc":"Primary Language"                                },
   {"type":"IS",  "desc":"Living Arrangement"                              },
   {"type":"CE",  "desc":"Publicity Code"                                  },
   {"type":"ID",  "desc":"Protection Indicator"                            },
   {"type":"IS",  "desc":"Student Indicator"                               },
   {"type":"CE",  "desc":"Religion"                                        },
   {"type":"XPN", "desc":"Mother's Maiden Name"                             ,"repeats":true},
   {"type":"CE",  "desc":"Nationality"                                     },
   {"type":"CE",  "desc":"Ethnic Group"                                     ,"repeats":true},
   {"type":"CE",  "desc":"Contact Reason"                                   ,"repeats":true},
   {"type":"XPN", "desc":"Contact Person's Name"                            ,"repeats":true},
   {"type":"XTN", "desc":"Contact Person's Telephone Number"                ,"repeats":true},
   {"type":"XAD", "desc":"Contact Person's Address"                         ,"repeats":true},
   {"type":"CX",  "desc":"Next Of Kin/associated Party's Identifiers"       ,"repeats":true},
   {"type":"IS",  "desc":"Job Status"                                      },
   {"type":"CE",  "desc":"Race"                                             ,"repeats":true},
   {"type":"IS",  "desc":"Handicap"                                        },
   {"type":"ST",  "desc":"Contact Person Social Security Number"           },
   {"type":"ST",  "desc":"Next Of Kin Birth Place"                         },
   {"type":"IS",  "desc":"Vip Indicator"                                   }]},
 "NK1_TIMING_QTY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "NK1_TIMING_QTY_ASSOCIATED_RX_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "NM":{
  "desc":"Numeric",
  "type":"composite"
 },
 "NMDN02":{
  "desc":"Application management data message (unsolicited)",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment" ,"repeats":true},
   {"type":"CLOCK_AND_STATS_WITH_NOTES", "desc":""                 ,"req":true ,"repeats":true}]},
 "NMQN01":{
  "desc":"Application management query message",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"   ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment" ,"repeats":true},
   {"type":"QRY_WITH_DETAIL",      "desc":""                },
   {"type":"CLOCK_AND_STATISTICS", "desc":""                 ,"req":true ,"repeats":true}]},
 "NMRN01":{
  "desc":"Application management query message - response",
  "type":"message",
  "children":[
   {"type":"MSH",                            "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                            "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA",                            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",                            "desc":"Error"                  ,"repeats":true},
   {"type":"QRD",                            "desc":"Style Query Definition"},
   {"type":"CLOCK_AND_STATS_WITH_NOTES_ALT", "desc":""                       ,"req":true ,"repeats":true}]},
 "NOTIFICATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NDS", "desc":"Notification Detail" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" }]},
 "NPU":{
  "desc":"Bed Status Update",
  "type":"segment",
  "children":[
   {"type":"PL", "desc":"Bed Location" ,"req":true},
   {"type":"IS", "desc":"Bed Status"  }]},
 "NR":{
  "desc":"Numeric Range",
  "type":"composite",
  "children":[
   {"type":"NM", "desc":"Low Value" },
   {"type":"NM", "desc":"High Value"}]},
 "NSC":{
  "desc":"Application Status Change",
  "type":"segment",
  "children":[
   {"type":"IS", "desc":"Application Change Type" ,"req":true},
   {"type":"ST", "desc":"Current Cpu"            },
   {"type":"ST", "desc":"Current Fileserver"     },
   {"type":"HD", "desc":"Current Application"    },
   {"type":"HD", "desc":"Current Facility"       },
   {"type":"ST", "desc":"New Cpu"                },
   {"type":"ST", "desc":"New Fileserver"         },
   {"type":"HD", "desc":"New Application"        },
   {"type":"HD", "desc":"New Facility"           }]},
 "NST":{
  "desc":"Application Control Level Statistics",
  "type":"segment",
  "children":[
   {"type":"ID", "desc":"Statistics Available"             ,"req":true},
   {"type":"ST", "desc":"Source Identifier"               },
   {"type":"ID", "desc":"Source Type"                     },
   {"type":"TS", "desc":"Statistics Start"                },
   {"type":"TS", "desc":"Statistics End"                  },
   {"type":"NM", "desc":"Receive Character Count"         },
   {"type":"NM", "desc":"Send Character Count"            },
   {"type":"NM", "desc":"Messages Received"               },
   {"type":"NM", "desc":"Messages Sent"                   },
   {"type":"NM", "desc":"Checksum Errors Received"        },
   {"type":"NM", "desc":"Length Errors Received"          },
   {"type":"NM", "desc":"Other Errors Received"           },
   {"type":"NM", "desc":"Connect Timeouts"                },
   {"type":"NM", "desc":"Receive Timeouts"                },
   {"type":"NM", "desc":"Application Control-level Errors"}]},
 "NTE":{
  "desc":"Notes And Comments",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Nte"     },
   {"type":"ID", "desc":"Source Of Comment"},
   {"type":"FT", "desc":"Comment"           ,"repeats":true},
   {"type":"CE", "desc":"Comment Type"     }]},
 "OBR":{
  "desc":"Observation Request",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Obr"                                   },
   {"type":"EI",  "desc":"Placer Order Number"                            },
   {"type":"EI",  "desc":"Filler Order Number"                            },
   {"type":"CE",  "desc":"Universal Service Identifier"                    ,"req":true},
   {"type":"ID",  "desc":"Priority _ Obr"                                 },
   {"type":"TS",  "desc":"Requested Date/time"                            },
   {"type":"TS",  "desc":"Observation Date/time"                          },
   {"type":"TS",  "desc":"Observation End Date/time"                      },
   {"type":"CQ",  "desc":"Collection Volume"                              },
   {"type":"XCN", "desc":"Collector Identifier"                            ,"repeats":true},
   {"type":"ID",  "desc":"Specimen Action Code"                           },
   {"type":"CE",  "desc":"Danger Code"                                    },
   {"type":"ST",  "desc":"Relevant Clinical Information"                  },
   {"type":"TS",  "desc":"Specimen Received Date/time"                    },
   {"type":"SPS", "desc":"Specimen Source"                                },
   {"type":"XCN", "desc":"Ordering Provider"                               ,"repeats":true},
   {"type":"XTN", "desc":"Order Callback Phone Number"                     ,"repeats":true},
   {"type":"ST",  "desc":"Placer Field 1"                                 },
   {"type":"ST",  "desc":"Placer Field 2"                                 },
   {"type":"ST",  "desc":"Filler Field 1"                                 },
   {"type":"ST",  "desc":"Filler Field 2"                                 },
   {"type":"TS",  "desc":"Results Rpt/status Chng - Date/time"            },
   {"type":"MOC", "desc":"Charge To Practice"                             },
   {"type":"ID",  "desc":"Diagnostic Serv Sect ID"                        },
   {"type":"ID",  "desc":"Result Status"                                  },
   {"type":"PRL", "desc":"Parent Result"                                  },
   {"type":"TQ",  "desc":"Quantity/timing"                                 ,"repeats":true},
   {"type":"XCN", "desc":"Result Copies To"                                ,"repeats":true},
   {"type":"EIP", "desc":"Parent"                                         },
   {"type":"ID",  "desc":"Transportation Mode"                            },
   {"type":"CE",  "desc":"Reason For Study"                                ,"repeats":true},
   {"type":"NDL", "desc":"Principal Result Interpreter"                   },
   {"type":"NDL", "desc":"Assistant Result Interpreter"                    ,"repeats":true},
   {"type":"NDL", "desc":"Technician"                                      ,"repeats":true},
   {"type":"NDL", "desc":"Transcriptionist"                                ,"repeats":true},
   {"type":"TS",  "desc":"Scheduled Date/time"                            },
   {"type":"NM",  "desc":"Number Of Sample Containers *"                  },
   {"type":"CE",  "desc":"Transport Logistics Of Collected Sample"         ,"repeats":true},
   {"type":"CE",  "desc":"Collector's Comment *"                           ,"repeats":true},
   {"type":"CE",  "desc":"Transport Arrangement Responsibility"           },
   {"type":"ID",  "desc":"Transport Arranged"                             },
   {"type":"ID",  "desc":"Escort Required"                                },
   {"type":"CE",  "desc":"Planned Patient Transport Comment"               ,"repeats":true},
   {"type":"CE",  "desc":"Procedure Code"                                 },
   {"type":"CE",  "desc":"Procedure Code Modifier"                         ,"repeats":true},
   {"type":"CE",  "desc":"Placer Supplemental Service Information"         ,"repeats":true},
   {"type":"CE",  "desc":"Filler Supplemental Service Information"         ,"repeats":true},
   {"type":"CWE", "desc":"Medically Necessary Duplicate Procedure Reason."},
   {"type":"IS",  "desc":"Result Handling"                                }]},
 "OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ADMINISTRATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_COMMON_ORDER_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_COMMON_ORDER_QUERY_RESPONSE_RSPZ86":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_COMMON_ORDER_QUERY_RESPONSE_RSPZ88":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_COMMON_ORDER_QUERY_RESPONSE_RSPZ90":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_DIET":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_GIVE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_GOAL_PGLPC7":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_GOAL_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"TCD", "desc":"Test Code Detail"  },
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_OBSERVATION_REQUEST_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"TCD", "desc":"Test Code Detail"  },
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_OBSERVATION_REQUEST_ORDER_SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"TCD", "desc":"Test Code Detail"  },
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_OBSERVATION_OULR21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"  },
   {"type":"TCD", "desc":"Test Code Detail"    },
   {"type":"SID", "desc":"Substance Identifier" ,"repeats":true},
   {"type":"NTE", "desc":"Notes And Comments"   ,"repeats":true}]},
 "OBSERVATION_ORDER_OMGO19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_OMIO23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_OMNO07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_OMPO09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_OMSO05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_RDEO11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_RDSO13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_VXRV03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORDER_VXUV04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORUR30":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORUR31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_ORUR32":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_PRIOR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_PRIOR_ORDER_PRIOR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_PRIOR_ORDER_PRIOR_PRIOR_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_PRIOR_ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                              "desc":"Observation Request" ,"req":true},
   {"type":"TCD",                              "desc":"Test Code Detail"   },
   {"type":"NTE",                              "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"CTD",                              "desc":"Contact Data"       },
   {"type":"DG1",                              "desc":"Diagnosis"           ,"repeats":true},
   {"type":"OBSERVATION_OBSERVATION_REQUEST",  "desc":""                    ,"repeats":true},
   {"type":"SPECIMEN_OBSERVATION_REQUEST",     "desc":""                    ,"repeats":true},
   {"type":"PRIOR_RESULT_OBSERVATION_REQUEST", "desc":""                    ,"repeats":true}]},
 "OBSERVATION_REQUEST_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                                    "desc":"Observation Request" ,"req":true},
   {"type":"TCD",                                    "desc":"Test Code Detail"   },
   {"type":"NTE",                                    "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"DG1",                                    "desc":"Diagnosis"           ,"repeats":true},
   {"type":"OBSERVATION_OBSERVATION_REQUEST_ORDER",  "desc":""                    ,"repeats":true},
   {"type":"PRIOR_RESULT_OBSERVATION_REQUEST_ORDER", "desc":""                    ,"repeats":true}]},
 "OBSERVATION_REQUEST_ORDER_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                                "desc":"Observation Request" ,"req":true},
   {"type":"SPECIMEN_OBSERVATION_REQUEST_ORDER", "desc":""                    ,"repeats":true}]},
 "OBSERVATION_REQUEST_ORDER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                                         "desc":"Observation Request" ,"req":true},
   {"type":"SPECIMEN_OBSERVATION_REQUEST_ORDER_SPECIMEN", "desc":""                    ,"repeats":true}]},
 "OBSERVATION_REQUEST_ORDER_SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                                                       "desc":"Observation Request" ,"req":true},
   {"type":"TCD",                                                       "desc":"Test Code Detail"   },
   {"type":"NTE",                                                       "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"DG1",                                                       "desc":"Diagnosis"           ,"repeats":true},
   {"type":"OBSERVATION_OBSERVATION_REQUEST_ORDER_SPECIMEN_CONTAINER",  "desc":""                    ,"repeats":true},
   {"type":"PRIOR_RESULT_OBSERVATION_REQUEST_ORDER_SPECIMEN_CONTAINER", "desc":""                    ,"repeats":true}]},
 "OBX":{
  "desc":"Observation/Result",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Obx"                     },
   {"type":"ID",  "desc":"Value Type"                       },
   {"type":"CE",  "desc":"Observation Identifier"            ,"req":true},
   {"type":"ST",  "desc":"Observation Sub-id"               },
   {"type":"ST",  "desc":"Observation Value"                 ,"repeats":true},
   {"type":"CE",  "desc":"Units"                            },
   {"type":"ST",  "desc":"References Range"                 },
   {"type":"IS",  "desc":"Abnormal Flags"                    ,"repeats":true},
   {"type":"NM",  "desc":"Probability"                      },
   {"type":"ID",  "desc":"Nature Of Abnormal Test"           ,"repeats":true},
   {"type":"ID",  "desc":"Observation Result Status"         ,"req":true},
   {"type":"TS",  "desc":"Effective Date Of Reference Range"},
   {"type":"ST",  "desc":"User Defined Access Checks"       },
   {"type":"TS",  "desc":"Date/time Of The Observation"     },
   {"type":"CE",  "desc":"Producer's ID"                    },
   {"type":"XCN", "desc":"Responsible Observer"              ,"repeats":true},
   {"type":"CE",  "desc":"Observation Method"                ,"repeats":true},
   {"type":"EI",  "desc":"Equipment Instance Identifier"     ,"repeats":true},
   {"type":"TS",  "desc":"Date/time Of The Analysis"        }]},
 "OCD":{
  "desc":"Occurrence Code And Date",
  "type":"composite",
  "children":[
   {"type":"CNE", "desc":"Occurrence Code"},
   {"type":"DT",  "desc":"Occurrence Date"}]},
 "ODS":{
  "desc":"Dietary Orders, Supplements, And Preferences",
  "type":"segment",
  "children":[
   {"type":"ID", "desc":"Type"                                 ,"req":true},
   {"type":"CE", "desc":"Service Period"                       ,"repeats":true},
   {"type":"CE", "desc":"Diet, Supplement, Or Preference Code" ,"req":true ,"repeats":true},
   {"type":"ST", "desc":"Text Instruction"                     ,"repeats":true}]},
 "ODT":{
  "desc":"Diet Tray Instructions",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Tray Type"        ,"req":true},
   {"type":"CE", "desc":"Service Period"   ,"repeats":true},
   {"type":"ST", "desc":"Text Instruction"}]},
 "OM1":{
  "desc":"General Segment",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Sequence Number - Test/observation Master File"                   ,"req":true},
   {"type":"CE",  "desc":"Producer's Service/test/observation ID"                           ,"req":true},
   {"type":"ID",  "desc":"Permitted Data Types"                                             ,"repeats":true},
   {"type":"ID",  "desc":"Specimen Required"                                                ,"req":true},
   {"type":"CE",  "desc":"Producer ID"                                                      ,"req":true},
   {"type":"TX",  "desc":"Observation Description"                                         },
   {"type":"CE",  "desc":"Other Service/test/observation Ids For The Observation"          },
   {"type":"ST",  "desc":"Other Names"                                                      ,"req":true ,"repeats":true},
   {"type":"ST",  "desc":"Preferred Report Name For The Observation"                       },
   {"type":"ST",  "desc":"Preferred Short Name Or Mnemonic For Observation"                },
   {"type":"ST",  "desc":"Preferred Long Name For The Observation"                         },
   {"type":"ID",  "desc":"Orderability"                                                    },
   {"type":"CE",  "desc":"Identity Of Instrument Used To Perform This Study"                ,"repeats":true},
   {"type":"CE",  "desc":"Coded Representation Of Method"                                   ,"repeats":true},
   {"type":"ID",  "desc":"Portable Device Indicator"                                       },
   {"type":"CE",  "desc":"Observation Producing Department/section"                         ,"repeats":true},
   {"type":"XTN", "desc":"Telephone Number Of Section"                                     },
   {"type":"IS",  "desc":"Nature Of Service/test/observation"                               ,"req":true},
   {"type":"CE",  "desc":"Report Subheader"                                                },
   {"type":"ST",  "desc":"Report Display Order"                                            },
   {"type":"TS",  "desc":"Date/time Stamp For Any Change In Definition For The Observation"},
   {"type":"TS",  "desc":"Effective Date/time Of Change"                                   },
   {"type":"NM",  "desc":"Typical Turn-around Time"                                        },
   {"type":"NM",  "desc":"Processing Time"                                                 },
   {"type":"ID",  "desc":"Processing Priority"                                              ,"repeats":true},
   {"type":"ID",  "desc":"Reporting Priority"                                              },
   {"type":"CE",  "desc":"Outside Site(s) Where Observation May Be Performed"               ,"repeats":true},
   {"type":"XAD", "desc":"Address Of Outside Site(s)"                                       ,"repeats":true},
   {"type":"XTN", "desc":"Phone Number Of Outside Site"                                    },
   {"type":"CWE", "desc":"Confidentiality Code"                                            },
   {"type":"CE",  "desc":"Observations Required To Interpret The Observation"              },
   {"type":"TX",  "desc":"Interpretation Of Observations"                                  },
   {"type":"CE",  "desc":"Contraindications To Observations"                               },
   {"type":"CE",  "desc":"Reflex Tests/observations"                                        ,"repeats":true},
   {"type":"TX",  "desc":"Rules That Trigger Reflex Testing"                               },
   {"type":"CE",  "desc":"Fixed Canned Message"                                            },
   {"type":"TX",  "desc":"Patient Preparation"                                             },
   {"type":"CE",  "desc":"Procedure Medication"                                            },
   {"type":"TX",  "desc":"Factors That May Affect The Observation"                         },
   {"type":"ST",  "desc":"Service/test/observation Performance Schedule"                    ,"repeats":true},
   {"type":"TX",  "desc":"Description Of Test Methods"                                     },
   {"type":"CE",  "desc":"Kind Of Quantity Observed"                                       },
   {"type":"CE",  "desc":"Point Versus Interval"                                           },
   {"type":"TX",  "desc":"Challenge Information"                                           },
   {"type":"CE",  "desc":"Relationship Modifier"                                           },
   {"type":"CE",  "desc":"Target Anatomic Site Of Test"                                    },
   {"type":"CE",  "desc":"Modality Of Imaging Measurement"                                 }]},
 "OM2":{
  "desc":"Numeric Observation",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Sequence Number - Test/observation Master File"                },
   {"type":"CE",  "desc":"Units Of Measure"                                              },
   {"type":"NM",  "desc":"Range Of Decimal Precision"                                     ,"repeats":true},
   {"type":"CE",  "desc":"Corresponding Si Units Of Measure"                             },
   {"type":"TX",  "desc":"Si Conversion Factor"                                          },
   {"type":"RFR", "desc":"Reference (normal) Range - Ordinal And Continuous Observations" ,"repeats":true},
   {"type":"RFR", "desc":"Critical Range For Ordinal And Continuous Observations"         ,"repeats":true},
   {"type":"RFR", "desc":"Absolute Range For Ordinal And Continuous Observations"        },
   {"type":"DLT", "desc":"Delta Check Criteria"                                           ,"repeats":true},
   {"type":"NM",  "desc":"Minimum Meaningful Increments"                                 }]},
 "OM3":{
  "desc":"Categorical Service/test/observation",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Sequence Number - Test/observation Master File"  },
   {"type":"CE", "desc":"Preferred Coding System"                         },
   {"type":"CE", "desc":"Valid Coded \"answers\""                         },
   {"type":"CE", "desc":"Normal Text/codes For Categorical Observations"   ,"repeats":true},
   {"type":"CE", "desc":"Abnormal Text/codes For Categorical Observations" ,"repeats":true},
   {"type":"CE", "desc":"Critical Text/codes For Categorical Observations" ,"repeats":true},
   {"type":"ID", "desc":"Value Type"                                      }]},
 "OM4":{
  "desc":"Observations That Require Specimens",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Sequence Number - Test/observation Master File"},
   {"type":"ID",  "desc":"Derived Specimen"                              },
   {"type":"TX",  "desc":"Container Description"                         },
   {"type":"NM",  "desc":"Container Volume"                              },
   {"type":"CE",  "desc":"Container Units"                               },
   {"type":"CE",  "desc":"Specimen"                                      },
   {"type":"CWE", "desc":"Additive"                                      },
   {"type":"TX",  "desc":"Preparation"                                   },
   {"type":"TX",  "desc":"Special Handling Requirements"                 },
   {"type":"CQ",  "desc":"Normal Collection Volume"                      },
   {"type":"CQ",  "desc":"Minimum Collection Volume"                     },
   {"type":"TX",  "desc":"Specimen Requirements"                         },
   {"type":"ID",  "desc":"Specimen Priorities"                            ,"repeats":true},
   {"type":"CQ",  "desc":"Specimen Retention Time"                       }]},
 "OM5":{
  "desc":"Observation Batteries (sets)",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Sequence Number - Test/observation Master File"           },
   {"type":"CE", "desc":"Test/observations Included Within An Ordered Test Battery" ,"repeats":true},
   {"type":"ST", "desc":"Observation ID Suffixes"                                  }]},
 "OM6":{
  "desc":"Observations That Are Calculated From Other Observations",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Sequence Number - Test/observation Master File"},
   {"type":"TX", "desc":"Derivation Rule"                               }]},
 "OM7":{
  "desc":"Additional Basic Attributes",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Sequence Number - Test/observation Master File" ,"req":true},
   {"type":"CE",  "desc":"Universal Service Identifier"                   ,"req":true},
   {"type":"CE",  "desc":"Category Identifier"                            ,"repeats":true},
   {"type":"TX",  "desc":"Category Description"                          },
   {"type":"ST",  "desc":"Category Synonym"                               ,"repeats":true},
   {"type":"TS",  "desc":"Effective Test/service Start Date/time"        },
   {"type":"TS",  "desc":"Effective Test/service End Date/time"          },
   {"type":"NM",  "desc":"Test/service Default Duration Quantity"        },
   {"type":"CE",  "desc":"Test/service Default Duration Units"           },
   {"type":"IS",  "desc":"Test/service Default Frequency"                },
   {"type":"ID",  "desc":"Consent Indicator"                             },
   {"type":"CE",  "desc":"Consent Identifier"                            },
   {"type":"TS",  "desc":"Consent Effective Start Date/time"             },
   {"type":"TS",  "desc":"Consent Effective End Date/time"               },
   {"type":"NM",  "desc":"Consent Interval Quantity"                     },
   {"type":"CE",  "desc":"Consent Interval Units"                        },
   {"type":"NM",  "desc":"Consent Waiting Period Quantity"               },
   {"type":"CE",  "desc":"Consent Waiting Period Units"                  },
   {"type":"TS",  "desc":"Effective Date/time Of Change"                 },
   {"type":"XCN", "desc":"Entered By"                                    },
   {"type":"PL",  "desc":"Orderable-at Location"                          ,"repeats":true},
   {"type":"IS",  "desc":"Formulary Status"                              },
   {"type":"ID",  "desc":"Special Order Indicator"                       },
   {"type":"CE",  "desc":"Primary Key Value - Cdm"                        ,"repeats":true}]},
 "OMBO27":{
  "desc":"Blood product order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMBO27", "desc":""                  },
   {"type":"ORDER_OMBO27",   "desc":""                   ,"req":true ,"repeats":true}]},
 "OMDO03":{
  "desc":"Diet order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMDO03", "desc":""                  },
   {"type":"ORDER_DIET",     "desc":""                   ,"req":true ,"repeats":true},
   {"type":"ORDER_TRAY",     "desc":""                   ,"repeats":true}]},
 "OMGO19":{
  "desc":"General clinical order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMGO19", "desc":""                  },
   {"type":"ORDER_OMGO19",   "desc":""                   ,"req":true ,"repeats":true}]},
 "OMIO23":{
  "desc":"Imaging order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMIO23", "desc":""                  },
   {"type":"ORDER_OMIO23",   "desc":""                   ,"req":true ,"repeats":true}]},
 "OMLO21":{
  "desc":"Laboratory order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMLO21", "desc":""                  },
   {"type":"ORDER_OMLO21",   "desc":""                   ,"req":true ,"repeats":true}]},
 "OMLO33":{
  "desc":"Laboratory order for multiple orders related to a single specimen",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"     ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMLO33",  "desc":""                  },
   {"type":"SPECIMEN_OMLO33", "desc":""                   ,"req":true ,"repeats":true}]},
 "OMLO35":{
  "desc":"Laboratory order for multiple orders related to a single container of a specimen",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"     ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMLO35",  "desc":""                  },
   {"type":"SPECIMEN_OMLO35", "desc":""                   ,"req":true ,"repeats":true}]},
 "OMNO07":{
  "desc":"Stock requisition order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMNO07", "desc":""                  },
   {"type":"ORDER_OMNO07",   "desc":""                   ,"req":true ,"repeats":true}]},
 "OMPO09":{
  "desc":"Pharmacy/treatment order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMPO09", "desc":""                  },
   {"type":"ORDER_OMPO09",   "desc":""                   ,"req":true ,"repeats":true}]},
 "OMSO05":{
  "desc":"Stock requisition order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_OMSO05", "desc":""                  },
   {"type":"ORDER_OMSO05",   "desc":""                   ,"req":true ,"repeats":true}]},
 "ORBO28":{
  "desc":"Blood product order acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"PATIENT_ORBO28", "desc":""                       ,"repeats":true}]},
 "ORC":{
  "desc":"Common Order",
  "type":"segment",
  "children":[
   {"type":"ID",  "desc":"Order Control"                               ,"req":true},
   {"type":"EI",  "desc":"Placer Order Number"                        },
   {"type":"EI",  "desc":"Filler Order Number"                        },
   {"type":"EI",  "desc":"Placer Group Number"                        },
   {"type":"ID",  "desc":"Order Status"                               },
   {"type":"ID",  "desc":"Response Flag"                              },
   {"type":"TQ",  "desc":"Quantity/timing"                             ,"repeats":true},
   {"type":"EIP", "desc":"Parent"                                     },
   {"type":"TS",  "desc":"Date/time Of Transaction"                   },
   {"type":"XCN", "desc":"Entered By"                                  ,"repeats":true},
   {"type":"XCN", "desc":"Verified By"                                 ,"repeats":true},
   {"type":"XCN", "desc":"Ordering Provider"                           ,"repeats":true},
   {"type":"PL",  "desc":"Enterer's Location"                         },
   {"type":"XTN", "desc":"Call Back Phone Number"                      ,"repeats":true},
   {"type":"TS",  "desc":"Order Effective Date/time"                  },
   {"type":"CE",  "desc":"Order Control Code Reason"                  },
   {"type":"CE",  "desc":"Entering Organization"                      },
   {"type":"CE",  "desc":"Entering Device"                            },
   {"type":"XCN", "desc":"Action By"                                   ,"repeats":true},
   {"type":"CE",  "desc":"Advanced Beneficiary Notice Code"           },
   {"type":"XON", "desc":"Ordering Facility Name"                      ,"repeats":true},
   {"type":"XAD", "desc":"Ordering Facility Address"                   ,"repeats":true},
   {"type":"XTN", "desc":"Ordering Facility Phone Number"              ,"repeats":true},
   {"type":"XAD", "desc":"Ordering Provider Address"                   ,"repeats":true},
   {"type":"CWE", "desc":"Order Status Modifier"                      },
   {"type":"CWE", "desc":"Advanced Beneficiary Notice Override Reason"},
   {"type":"TS",  "desc":"Filler's Expected Availability Date/time"   },
   {"type":"CWE", "desc":"Confidentiality Code"                       },
   {"type":"CWE", "desc":"Order Type"                                 },
   {"type":"CNE", "desc":"Enterer Authorization Mode"                 }]},
 "ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",     "desc":"Common Order"        ,"req":true},
   {"type":"TIMING",  "desc":""                    ,"repeats":true},
   {"type":"BPO",     "desc":"Blood Product Order" ,"req":true},
   {"type":"NTE",     "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"PRODUCT", "desc":""                    ,"repeats":true}]},
 "ORDER_BTSO31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                 "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_ORDER_BTSO31", "desc":""                    ,"repeats":true},
   {"type":"BPO",                 "desc":"Blood Product Order" ,"req":true},
   {"type":"NTE",                 "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"PRODUCT_STATUS",      "desc":""                    ,"repeats":true}]},
 "ORDER_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"  ,"repeats":true}]},
 "ORDER_COMMON_ORDER_DFTP11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR", "desc":"Observation Request" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"  ,"repeats":true}]},
 "ORDER_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                        "desc":"Observation Request"           ,"req":true},
   {"type":"ORC",                        "desc":"Common Order"                 },
   {"type":"NTE",                        "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"TIMING_QTY_ORDER_CONTAINER", "desc":""                              ,"repeats":true},
   {"type":"RESULT_ORDER_CONTAINER",     "desc":""                              ,"repeats":true},
   {"type":"CTI",                        "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_DEFINITION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",      "desc":"Common Order"                      ,"req":true},
   {"type":"ENCODING", "desc":""                                 },
   {"type":"RXA",      "desc":"Pharmacy/Treatment Administration" ,"req":true ,"repeats":true},
   {"type":"RXR",      "desc":"Pharmacy/Treatment Route"          ,"req":true}]},
 "ORDER_DEFINITION_RDRRDR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                       "desc":"Common Order" ,"req":true},
   {"type":"ENCODING_ORDER_DEFINITION", "desc":""            },
   {"type":"DISPENSE",                  "desc":""             ,"req":true ,"repeats":true}]},
 "ORDER_DEFINITION_RERRER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC", "desc":"Common Order"                       ,"req":true},
   {"type":"RXE", "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ORDER_DEFINITION_RGRRGR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                              "desc":"Common Order"                       ,"req":true},
   {"type":"ENCODING_ORDER_DEFINITION_RGRRGR", "desc":""                                  },
   {"type":"RXG",                              "desc":"Pharmacy/Treatment Give"            ,"req":true ,"repeats":true},
   {"type":"RXR",                              "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                              "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ORDER_DEFINITION_RORROR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC", "desc":"Common Order"                       ,"req":true},
   {"type":"RXO", "desc":"Pharmacy/Treatment Order"           ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ORDER_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"Group6",                   "desc":""                   ,"repeats":true},
   {"type":"NTE",                      "desc":"Notes And Comments" ,"repeats":true},
   {"type":"CTD",                      "desc":"Contact Data"      },
   {"type":"DG1",                      "desc":"Diagnosis"          ,"repeats":true},
   {"type":"OBSERVATION_ORDER_DETAIL", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",       "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"NTE",       "desc":"Notes And Comments"       ,"repeats":true},
   {"type":"RXR",       "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"TREATMENT", "desc":""                        }]},
 "ORDER_DETAIL_COMMON_ORDER_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO", "desc":"Pharmacy/Treatment Order"           ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ORDER_DETAIL_COMMON_ORDER_QUERY_RESPONSE_RSPZ88":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                                 "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"NTE",                                 "desc":"Notes And Comments"       ,"repeats":true},
   {"type":"RXR",                                 "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENT_ORDER_DETAIL_COMMON_ORDER", "desc":""                        }]},
 "ORDER_DETAIL_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                    "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"NTE",                    "desc":"Notes And Comments"       ,"repeats":true},
   {"type":"RXR",                    "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENT_ORDER_DETAIL", "desc":""                         ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                            "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                               "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                               "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PATIENT",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                       "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                       "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PATIENT", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCH",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                      "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                      "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCH", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCJ",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                      "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                      "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCJ", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"Group12",                                           "desc":""                   ,"repeats":true},
   {"type":"NTE",                                               "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                               "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATIENT", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL_PGLPC7":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                  "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                  "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_GOAL_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_GOAL",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                       "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                       "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                          "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                          "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                  "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                  "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PATIENT",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                          "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                          "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PATIENT", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PPPPCD",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                         "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                         "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PPPPCD", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PATIENT",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                  "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                  "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATIENT", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PPRPC1",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                 "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                 "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PPRPC1", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PPRPC2",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                 "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                 "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PPRPC2", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CHOICE_ORDER_DETAIL_ORDER_PROBLEM_PPRPC3",            "desc":""                   ,"repeats":true},
   {"type":"NTE",                                                 "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                                 "desc":"Variance"           ,"repeats":true},
   {"type":"ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PPRPC3", "desc":""                   ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_RASO17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                     "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"ORDER_DETAIL_SUPPLEMENT", "desc":""                        }]},
 "ORDER_DETAIL_ORDER_RDEO11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                          "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"NTE",                          "desc":"Notes And Comments"       ,"repeats":true},
   {"type":"RXR",                          "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENT_ORDER_DETAIL_ORDER", "desc":""                         ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                                 "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"NTE",                                 "desc":"Notes And Comments"       ,"repeats":true},
   {"type":"RXR",                                 "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENT_ORDER_DETAIL_ORDER_RDEO25", "desc":""                         ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_RDSO13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                                  "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"ORDER_DETAIL_SUPPLEMENT_ORDER_DETAIL", "desc":""                        }]},
 "ORDER_DETAIL_ORDER_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                     "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"NTE",                     "desc":"Notes And Comments"       ,"repeats":true},
   {"type":"RXR",                     "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENTS_ORDER_DETAIL", "desc":""                         ,"repeats":true}]},
 "ORDER_DETAIL_ORDER_RGVO15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXO",                                        "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"ORDER_DETAIL_SUPPLEMENT_ORDER_DETAIL_ORDER", "desc":""                        }]},
 "ORDER_DETAIL_SUPPLEMENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NTE",        "desc":"Notes And Comments"       ,"req":true ,"repeats":true},
   {"type":"RXR",        "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENTS", "desc":""                         ,"repeats":true}]},
 "ORDER_DETAIL_SUPPLEMENT_ORDER_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NTE",                               "desc":"Notes And Comments"       ,"req":true ,"repeats":true},
   {"type":"RXR",                               "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENT_ORDER_DETAIL_SUPPLEMENT", "desc":""                         ,"repeats":true}]},
 "ORDER_DETAIL_SUPPLEMENT_ORDER_DETAIL_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"NTE",                                "desc":"Notes And Comments"       ,"req":true ,"repeats":true},
   {"type":"RXR",                                "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENTS_ORDER_DETAIL_SUPPLEMENT", "desc":""                         ,"repeats":true}]},
 "ORDER_DIET":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",         "desc":"Common Order" ,"req":true},
   {"type":"TIMING_DIET", "desc":""             ,"repeats":true},
   {"type":"DIET",        "desc":""            }]},
 "ORDER_DIET_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                    "desc":"Common Order"                                 ,"req":true},
   {"type":"TIMING_DIET_ORDER_DIET", "desc":""                                             ,"repeats":true},
   {"type":"ODS",                    "desc":"Dietary Orders, Supplements, And Preferences" ,"repeats":true},
   {"type":"NTE",                    "desc":"Notes And Comments"                           ,"repeats":true}]},
 "ORDER_ENCODED":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                          "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"TIMING_ENCODED_ORDER_ENCODED", "desc":""                                   ,"repeats":true},
   {"type":"RXR",                          "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                          "desc":"Pharmacy/Treatment Component Order" ,"repeats":true}]},
 "ORDER_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                     "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL", "desc":""            }]},
 "ORDER_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                             "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL_PATHWAY", "desc":""            }]},
 "ORDER_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                     "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL_PATHWAY_PATIENT", "desc":""            }]},
 "ORDER_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                    "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCH", "desc":""            }]},
 "ORDER_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                    "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCJ", "desc":""            }]},
 "ORDER_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                             "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL_PATIENT", "desc":""            }]},
 "ORDER_GOAL_PGLPC7":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                            "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL_PGLPC7", "desc":""            }]},
 "ORDER_GOAL_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                            "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_GOAL_PGLPC8", "desc":""            }]},
 "ORDER_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                           "desc":"Common Order"                 },
   {"type":"OBR",                           "desc":"Observation Request"           ,"req":true},
   {"type":"NTE",                           "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"TIMING_QTY_ORDER_OBSERVATION",  "desc":""                              ,"repeats":true},
   {"type":"CTD",                           "desc":"Contact Data"                 },
   {"type":"OBSERVATION_ORDER_OBSERVATION", "desc":""                              ,"repeats":true},
   {"type":"FT1",                           "desc":"Financial Transaction"         ,"repeats":true},
   {"type":"CTI",                           "desc":"Clinical Trial Identification" ,"repeats":true},
   {"type":"SPECIMEN_ORDER_OBSERVATION",    "desc":""                              ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_ORDER_DETAIL_ORDER_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR", "desc":"Variance"           ,"repeats":true}]},
 "ORDER_OBSERVATION_OULR21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CONTAINER_ORDER_OBSERVATION",          "desc":""                             },
   {"type":"ORC",                                  "desc":"Common Order"                 },
   {"type":"OBR",                                  "desc":"Observation Request"           ,"req":true},
   {"type":"NTE",                                  "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"TIMING_QTY_ORDER_OBSERVATION_OULR21",  "desc":""                              ,"repeats":true},
   {"type":"OBSERVATION_ORDER_OBSERVATION_OULR21", "desc":""                              ,"req":true ,"repeats":true},
   {"type":"CTI",                                  "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_OMBO27":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                 "desc":"Common Order"          ,"req":true},
   {"type":"TIMING_ORDER_OMBO27", "desc":""                      ,"repeats":true},
   {"type":"BPO",                 "desc":"Blood Product Order"   ,"req":true},
   {"type":"SPM",                 "desc":"Specimen"             },
   {"type":"NTE",                 "desc":"Notes And Comments"    ,"repeats":true},
   {"type":"DG1",                 "desc":"Diagnosis"             ,"repeats":true},
   {"type":"OBSERVATION_ORDER",   "desc":""                      ,"repeats":true},
   {"type":"FT1",                 "desc":"Financial Transaction" ,"repeats":true},
   {"type":"BLG",                 "desc":"Billing"              }]},
 "ORDER_OMGO19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                      "desc":"Common Order"                  ,"req":true},
   {"type":"TIMING_ORDER_OMGO19",      "desc":""                              ,"repeats":true},
   {"type":"OBR",                      "desc":"Observation Request"           ,"req":true},
   {"type":"NTE",                      "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"CTD",                      "desc":"Contact Data"                 },
   {"type":"DG1",                      "desc":"Diagnosis"                     ,"repeats":true},
   {"type":"OBSERVATION_ORDER_OMGO19", "desc":""                              ,"repeats":true},
   {"type":"SPECIMEN",                 "desc":""                              ,"repeats":true},
   {"type":"PRIOR_RESULT",             "desc":""                              ,"repeats":true},
   {"type":"FT1",                      "desc":"Financial Transaction"         ,"repeats":true},
   {"type":"CTI",                      "desc":"Clinical Trial Identification" ,"repeats":true},
   {"type":"BLG",                      "desc":"Billing"                      }]},
 "ORDER_OMIO23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                      "desc":"Common Order"                      ,"req":true},
   {"type":"TIMING_ORDER_OMIO23",      "desc":""                                  ,"repeats":true},
   {"type":"OBR",                      "desc":"Observation Request"               ,"req":true},
   {"type":"NTE",                      "desc":"Notes And Comments"                ,"repeats":true},
   {"type":"CTD",                      "desc":"Contact Data"                     },
   {"type":"DG1",                      "desc":"Diagnosis"                         ,"repeats":true},
   {"type":"OBSERVATION_ORDER_OMIO23", "desc":""                                  ,"repeats":true},
   {"type":"IPC",                      "desc":"Imaging Procedure Control Segment" ,"req":true ,"repeats":true}]},
 "ORDER_OMLO21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                 "desc":"Common Order"                  ,"req":true},
   {"type":"TIIMING",             "desc":""                              ,"repeats":true},
   {"type":"OBSERVATION_REQUEST", "desc":""                             },
   {"type":"FT1",                 "desc":"Financial Transaction"         ,"repeats":true},
   {"type":"CTI",                 "desc":"Clinical Trial Identification" ,"repeats":true},
   {"type":"BLG",                 "desc":"Billing"                      }]},
 "ORDER_OMNO07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                      "desc":"Common Order"                                ,"req":true},
   {"type":"TIMING_ORDER_OMNO07",      "desc":""                                            ,"repeats":true},
   {"type":"RQD",                      "desc":"Requisition Detail"                          ,"req":true},
   {"type":"RQ1",                      "desc":"Requisition Detail - Additional Information"},
   {"type":"NTE",                      "desc":"Notes And Comments"                          ,"repeats":true},
   {"type":"OBSERVATION_ORDER_OMNO07", "desc":""                                            ,"repeats":true},
   {"type":"BLG",                      "desc":"Billing"                                    }]},
 "ORDER_OMPO09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                      "desc":"Common Order"             ,"req":true},
   {"type":"TIMING_ORDER_OMPO09",      "desc":""                         ,"repeats":true},
   {"type":"RXO",                      "desc":"Pharmacy/Treatment Order" ,"req":true},
   {"type":"NTE",                      "desc":"Notes And Comments"       ,"repeats":true},
   {"type":"RXR",                      "desc":"Pharmacy/Treatment Route" ,"req":true ,"repeats":true},
   {"type":"COMPONENT",                "desc":""                         ,"repeats":true},
   {"type":"OBSERVATION_ORDER_OMPO09", "desc":""                         ,"repeats":true},
   {"type":"FT1",                      "desc":"Financial Transaction"    ,"repeats":true},
   {"type":"BLG",                      "desc":"Billing"                 }]},
 "ORDER_OMSO05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                      "desc":"Common Order"                                ,"req":true},
   {"type":"TIMING_ORDER_OMSO05",      "desc":""                                            ,"repeats":true},
   {"type":"RQD",                      "desc":"Requisition Detail"                          ,"req":true},
   {"type":"RQ1",                      "desc":"Requisition Detail - Additional Information"},
   {"type":"NTE",                      "desc":"Notes And Comments"                          ,"repeats":true},
   {"type":"OBSERVATION_ORDER_OMSO05", "desc":""                                            ,"repeats":true},
   {"type":"BLG",                      "desc":"Billing"                                    }]},
 "ORDER_ORMO01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",          "desc":"Common Order"                  ,"req":true},
   {"type":"ORDER_DETAIL", "desc":""                             },
   {"type":"FT1",          "desc":"Financial Transaction"         ,"repeats":true},
   {"type":"CTI",          "desc":"Clinical Trial Identification" ,"repeats":true},
   {"type":"BLG",          "desc":"Billing"                      }]},
 "ORDER_OULR24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                     "desc":"Observation Request"           ,"req":true},
   {"type":"ORC",                     "desc":"Common Order"                 },
   {"type":"NTE",                     "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"TIMING_QTY_ORDER_OULR24", "desc":""                              ,"repeats":true},
   {"type":"SPECIMEN_ORDER_OULR24",   "desc":""                              ,"repeats":true},
   {"type":"RESULT_ORDER_OULR24",     "desc":""                              ,"repeats":true},
   {"type":"CTI",                     "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",          "desc":"Common Order"                  ,"req":true},
   {"type":"TIMING_ORDER", "desc":""                              ,"repeats":true},
   {"type":"BPO",          "desc":"Blood Product Order"          },
   {"type":"BPX",          "desc":"Blood Product Dispense Status" ,"repeats":true}]},
 "ORDER_PATIENT_ORBO28":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                  "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_ORDER_PATIENT", "desc":""                    ,"repeats":true},
   {"type":"BPO",                  "desc":"Blood Product Order"}]},
 "ORDER_PATIENT_ORLO22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                               "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_PATIENT_ORLO22",       "desc":""             ,"repeats":true},
   {"type":"OBSERVATION_REQUEST_ORDER_PATIENT", "desc":""            }]},
 "ORDER_PRIOR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",               "desc":"Common Order"       },
   {"type":"OBR",               "desc":"Observation Request" ,"req":true},
   {"type":"TIMING_PRIOR",      "desc":""                    ,"repeats":true},
   {"type":"NTE",               "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"CTD",               "desc":"Contact Data"       },
   {"type":"OBSERVATION_PRIOR", "desc":""                    ,"req":true ,"repeats":true}]},
 "ORDER_PRIOR_PRIOR_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                           "desc":"Common Order"       },
   {"type":"OBR",                           "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                           "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"TIMING_PRIOR_ORDER_PRIOR",      "desc":""                    ,"repeats":true},
   {"type":"OBSERVATION_PRIOR_ORDER_PRIOR", "desc":""                    ,"req":true ,"repeats":true}]},
 "ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                        "desc":"Common Order"       },
   {"type":"OBR",                                        "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                                        "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"TIMING_PRIOR_ORDER_PRIOR_PRIOR_RESULT",      "desc":""                    ,"repeats":true},
   {"type":"OBSERVATION_PRIOR_ORDER_PRIOR_PRIOR_RESULT", "desc":""                    ,"req":true ,"repeats":true}]},
 "ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                                            "desc":"Common Order"       },
   {"type":"OBR",                                                            "desc":"Observation Request" ,"req":true},
   {"type":"NTE",                                                            "desc":"Notes And Comments"  ,"repeats":true},
   {"type":"TIMING_PRIOR_ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST",      "desc":""                    ,"repeats":true},
   {"type":"OBSERVATION_PRIOR_ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST", "desc":""                    ,"req":true ,"repeats":true}]},
 "ORDER_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                        "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM", "desc":""            }]},
 "ORDER_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM_PATHWAY", "desc":""            }]},
 "ORDER_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                        "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PATIENT", "desc":""            }]},
 "ORDER_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                       "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM_PATHWAY_PPPPCD", "desc":""            }]},
 "ORDER_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM_PATIENT", "desc":""            }]},
 "ORDER_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                               "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM_PPRPC1", "desc":""            }]},
 "ORDER_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                               "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM_PPRPC2", "desc":""            }]},
 "ORDER_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                               "desc":"Common Order" ,"req":true},
   {"type":"ORDER_DETAIL_ORDER_PROBLEM_PPRPC3", "desc":""            }]},
 "ORDER_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                              "desc":"Common Order"                 },
   {"type":"OBR",                              "desc":"Observation Request"           ,"req":true},
   {"type":"NTE",                              "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"TIMING_QTY_ORDER",                 "desc":""                              ,"repeats":true},
   {"type":"CTD",                              "desc":"Contact Data"                 },
   {"type":"OBSERVATION_ORDER_QUERY_RESPONSE", "desc":""                              ,"req":true ,"repeats":true},
   {"type":"CTI",                              "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_RASO17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                       "desc":"Common Order"                  ,"req":true},
   {"type":"TIMING_ORDER_RASO17",       "desc":""                              ,"repeats":true},
   {"type":"ORDER_DETAIL_ORDER_RASO17", "desc":""                             },
   {"type":"ENCODING_ORDER",            "desc":""                             },
   {"type":"ADMINISTRATION",            "desc":""                              ,"req":true ,"repeats":true},
   {"type":"CTI",                       "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_RDEO11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                       "desc":"Common Order"                       ,"req":true},
   {"type":"TIMING_ORDER_RDEO11",       "desc":""                                   ,"repeats":true},
   {"type":"ORDER_DETAIL_ORDER_RDEO11", "desc":""                                  },
   {"type":"RXE",                       "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"NTE",                       "desc":"Notes And Comments"                 ,"repeats":true},
   {"type":"TIMING_ENCODED_ORDER",      "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",                       "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                       "desc":"Pharmacy/Treatment Component Order" ,"repeats":true},
   {"type":"OBSERVATION_ORDER_RDEO11",  "desc":""                                   ,"repeats":true},
   {"type":"FT1",                       "desc":"Financial Transaction"              ,"repeats":true},
   {"type":"BLG",                       "desc":"Billing"                           },
   {"type":"CTI",                       "desc":"Clinical Trial Identification"      ,"repeats":true}]},
 "ORDER_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                         "desc":"Common Order"                       ,"req":true},
   {"type":"TIMING_ORDER_RDEO25",         "desc":""                                   ,"repeats":true},
   {"type":"ORDER_DETAIL_ORDER_RDEO25",   "desc":""                                  },
   {"type":"RXE",                         "desc":"Pharmacy/Treatment Encoded Order"   ,"req":true},
   {"type":"NTE",                         "desc":"Notes And Comments"                 ,"repeats":true},
   {"type":"TIMING_ENCODED_ORDER_RDEO25", "desc":""                                   ,"req":true ,"repeats":true},
   {"type":"RXR",                         "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                         "desc":"Pharmacy/Treatment Component Order" ,"repeats":true},
   {"type":"OBSERVATION_ORDER_RDEO25",    "desc":""                                   ,"repeats":true},
   {"type":"FT1",                         "desc":"Financial Transaction"              ,"repeats":true},
   {"type":"BLG",                         "desc":"Billing"                           },
   {"type":"CTI",                         "desc":"Clinical Trial Identification"      ,"repeats":true}]},
 "ORDER_RDSO13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                       "desc":"Common Order"                       ,"req":true},
   {"type":"TIMING_ORDER_RDSO13",       "desc":""                                   ,"repeats":true},
   {"type":"ORDER_DETAIL_ORDER_RDSO13", "desc":""                                  },
   {"type":"ENCODING_ORDER_RDSO13",     "desc":""                                  },
   {"type":"RXD",                       "desc":"Pharmacy/Treatment Dispense"        ,"req":true},
   {"type":"NTE",                       "desc":"Notes And Comments"                 ,"repeats":true},
   {"type":"RXR",                       "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                       "desc":"Pharmacy/Treatment Component Order" ,"repeats":true},
   {"type":"OBSERVATION_ORDER_RDSO13",  "desc":""                                   ,"repeats":true},
   {"type":"FT1",                       "desc":"Financial Transaction"              ,"repeats":true}]},
 "ORDER_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                   "desc":"Common Order"                          ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE", "desc":""                                      ,"repeats":true},
   {"type":"BPO",                   "desc":"Blood Product Order"                  },
   {"type":"BTX",                   "desc":"Blood Product Transfusion/Disposition" ,"repeats":true}]},
 "ORDER_RESPONSE_ORGO20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order"                  ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_ORGO20", "desc":""                              ,"repeats":true},
   {"type":"OBR",                          "desc":"Observation Request"          },
   {"type":"NTE",                          "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"CTI",                          "desc":"Clinical Trial Identification" ,"repeats":true},
   {"type":"SPECIMEN_ORDER",               "desc":""                              ,"repeats":true}]},
 "ORDER_RESPONSE_ORIO24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order"                      ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_ORIO24", "desc":""                                  ,"repeats":true},
   {"type":"OBR",                          "desc":"Observation Request"               ,"req":true},
   {"type":"NTE",                          "desc":"Notes And Comments"                ,"repeats":true},
   {"type":"IPC",                          "desc":"Imaging Procedure Control Segment" ,"req":true ,"repeats":true}]},
 "ORDER_RESPONSE_ORNO08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order"                                ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_ORNO08", "desc":""                                            ,"repeats":true},
   {"type":"RQD",                          "desc":"Requisition Detail"                          ,"req":true},
   {"type":"RQ1",                          "desc":"Requisition Detail - Additional Information"},
   {"type":"NTE",                          "desc":"Notes And Comments"                          ,"repeats":true}]},
 "ORDER_RESPONSE_ORPO10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_ORPO10", "desc":""             ,"repeats":true},
   {"type":"ORDER_DETAIL_ORDER",           "desc":""            }]},
 "ORDER_RESPONSE_ORRO02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",    "desc":"Common Order"                  ,"req":true},
   {"type":"CHOICE", "desc":""                              ,"repeats":true},
   {"type":"NTE",    "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"CTI",    "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_RESPONSE_ORSO06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order"                                ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_ORSO06", "desc":""                                            ,"repeats":true},
   {"type":"RQD",                          "desc":"Requisition Detail"                          ,"req":true},
   {"type":"RQ1",                          "desc":"Requisition Detail - Additional Information"},
   {"type":"NTE",                          "desc":"Notes And Comments"                          ,"repeats":true}]},
 "ORDER_RESPONSE_OSRQ06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order"                  ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_OSRQ06", "desc":""                              ,"repeats":true},
   {"type":"CHOICE_ORDER",                 "desc":""                              ,"repeats":true},
   {"type":"NTE",                          "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"CTI",                          "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_RESPONSE_RRAO18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_RRAO18", "desc":""             ,"repeats":true},
   {"type":"ADMINISTRATION_ORDER",         "desc":""            }]},
 "ORDER_RESPONSE_RRDO14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_RRDO14", "desc":""             ,"repeats":true},
   {"type":"DISPENSE_ORDER",               "desc":""            }]},
 "ORDER_RESPONSE_RREO12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_RREO12", "desc":""             ,"repeats":true},
   {"type":"ENCODING_ORDER_RESPONSE",      "desc":""            }]},
 "ORDER_RESPONSE_RREO26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                            "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_RREO26",   "desc":""             ,"repeats":true},
   {"type":"ENCODING_ORDER_RESPONSE_RREO26", "desc":""            }]},
 "ORDER_RESPONSE_RRGO16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_RRGO16", "desc":""             ,"repeats":true},
   {"type":"GIVE_ORDER",                   "desc":""            }]},
 "ORDER_RESPONSE_RSPK31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                            "desc":"Common Order"                       ,"req":true},
   {"type":"TIMING_ORDER_RESPONSE_RSPK31",   "desc":""                                   ,"repeats":true},
   {"type":"ORDER_DETAIL_ORDER_RESPONSE",    "desc":""                                  },
   {"type":"ENCODING_ORDER_RESPONSE_RSPK31", "desc":""                                  },
   {"type":"RXD",                            "desc":"Pharmacy/Treatment Dispense"        ,"req":true},
   {"type":"RXR",                            "desc":"Pharmacy/Treatment Route"           ,"req":true ,"repeats":true},
   {"type":"RXC",                            "desc":"Pharmacy/Treatment Component Order" ,"repeats":true},
   {"type":"OBSERVATION_ORDER_RESPONSE",     "desc":""                                   ,"req":true ,"repeats":true}]},
 "ORDER_RGVO15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                       "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_RGVO15",       "desc":""             ,"repeats":true},
   {"type":"ORDER_DETAIL_ORDER_RGVO15", "desc":""            },
   {"type":"ENCODING_ORDER_RGVO15",     "desc":""            },
   {"type":"GIVE",                      "desc":""             ,"req":true ,"repeats":true}]},
 "ORDER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                       "desc":"Common Order"                  ,"req":true},
   {"type":"TIIMING_ORDER",             "desc":""                              ,"repeats":true},
   {"type":"OBSERVATION_REQUEST_ORDER", "desc":""                             },
   {"type":"FT1",                       "desc":"Financial Transaction"         ,"repeats":true},
   {"type":"CTI",                       "desc":"Clinical Trial Identification" ,"repeats":true},
   {"type":"BLG",                       "desc":"Billing"                      }]},
 "ORDER_SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                          "desc":"Common Order"                  ,"req":true},
   {"type":"TIIMING_ORDER_SPECIMEN_CONTAINER",             "desc":""                              ,"repeats":true},
   {"type":"OBSERVATION_REQUEST_ORDER_SPECIMEN_CONTAINER", "desc":""                             },
   {"type":"FT1",                                          "desc":"Financial Transaction"         ,"repeats":true},
   {"type":"CTI",                                          "desc":"Clinical Trial Identification" ,"repeats":true},
   {"type":"BLG",                                          "desc":"Billing"                      }]},
 "ORDER_SPECIMEN_CONTAINER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                             "desc":"Common Order"        ,"req":true},
   {"type":"TIMING_ORDER_SPECIMEN_CONTAINER", "desc":""                    ,"repeats":true},
   {"type":"OBR",                             "desc":"Observation Request" ,"req":true}]},
 "ORDER_SPECIMEN_OULR22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBR",                       "desc":"Observation Request"           ,"req":true},
   {"type":"ORC",                       "desc":"Common Order"                 },
   {"type":"NTE",                       "desc":"Notes And Comments"            ,"repeats":true},
   {"type":"TIMING_QTY_ORDER_SPECIMEN", "desc":""                              ,"repeats":true},
   {"type":"RESULT_ORDER",              "desc":""                              ,"repeats":true},
   {"type":"CTI",                       "desc":"Clinical Trial Identification" ,"repeats":true}]},
 "ORDER_SPECIMEN_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                "desc":"Common Order" ,"req":true},
   {"type":"TIMING_ORDER_SPECIMEN",              "desc":""             ,"repeats":true},
   {"type":"OBSERVATION_REQUEST_ORDER_SPECIMEN", "desc":""            }]},
 "ORDER_TRAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",         "desc":"Common Order"           ,"req":true},
   {"type":"TIMING_TRAY", "desc":""                       ,"repeats":true},
   {"type":"ODT",         "desc":"Diet Tray Instructions" ,"req":true ,"repeats":true},
   {"type":"NTE",         "desc":"Notes And Comments"     ,"repeats":true}]},
 "ORDER_TRAY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                    "desc":"Common Order"           ,"req":true},
   {"type":"TIMING_TRAY_ORDER_TRAY", "desc":""                       ,"repeats":true},
   {"type":"ODT",                    "desc":"Diet Tray Instructions" ,"repeats":true},
   {"type":"NTE",                    "desc":"Notes And Comments"     ,"repeats":true}]},
 "ORDER_VXRV03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                      "desc":"Common Order"                      ,"req":true},
   {"type":"TIMING_ORDER_VXRV03",      "desc":""                                  ,"repeats":true},
   {"type":"RXA",                      "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR",                      "desc":"Pharmacy/Treatment Route"         },
   {"type":"OBSERVATION_ORDER_VXRV03", "desc":""                                  ,"repeats":true}]},
 "ORDER_VXUV04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                      "desc":"Common Order"                      ,"req":true},
   {"type":"TIMING_ORDER_VXUV04",      "desc":""                                  ,"repeats":true},
   {"type":"RXA",                      "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR",                      "desc":"Pharmacy/Treatment Route"         },
   {"type":"OBSERVATION_ORDER_VXUV04", "desc":""                                  ,"repeats":true}]},
 "ORDO04":{
  "desc":"Diet order acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_ORDO04", "desc":""                      }]},
 "ORFR04":{
  "desc":"Response to query; transmission of requested observation",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"              ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"      ,"req":true},
   {"type":"QRD",                   "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",                   "desc":"Original Style Query Filter"},
   {"type":"QUERY_RESPONSE_ORFR04", "desc":""                            ,"req":true ,"repeats":true},
   {"type":"ERR",                   "desc":"Error"                       ,"repeats":true},
   {"type":"QAK",                   "desc":"Query Acknowledgment"       },
   {"type":"DSC",                   "desc":"Continuation Pointer"       }]},
 "ORG":{
  "desc":"Practitioner Organization Unit",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID _ Org"                                     ,"req":true},
   {"type":"CE", "desc":"Organization Unit Code"                          },
   {"type":"CE", "desc":"Organization Unit Type Code"                     },
   {"type":"ID", "desc":"Primary Org Unit Indicator"                      },
   {"type":"CX", "desc":"Practitioner Org Unit Identifier"                },
   {"type":"CE", "desc":"Health Care Provider Type Code"                  },
   {"type":"CE", "desc":"Health Care Provider Classification Code"        },
   {"type":"CE", "desc":"Health Care Provider Area Of Specialization Code"},
   {"type":"DR", "desc":"Effective Date Range"                            },
   {"type":"CE", "desc":"Employment Status Code"                          },
   {"type":"ID", "desc":"Board Approval Indicator"                        },
   {"type":"ID", "desc":"Primary Care Physician Indicator"                }]},
 "ORGO20":{
  "desc":"General clinical order response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_ORGO20", "desc":""                      }]},
 "ORIO24":{
  "desc":"Imaging order response message to any OMI",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_ORIO24", "desc":""                      }]},
 "ORLO22":{
  "desc":"General laboratory order response message to any OML",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"PATIENT_ORLO22", "desc":""                       ,"repeats":true}]},
 "ORLO34":{
  "desc":"Laboratory order response message to a multiple order related to single specimen",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"PATIENT_ORLO34", "desc":""                       ,"repeats":true}]},
 "ORLO36":{
  "desc":"Laboratory order response message to a single container of a specimen OML",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"PATIENT_ORLO36", "desc":""                       ,"repeats":true}]},
 "ORMO01":{
  "desc":"Order message (also RDE, RDS, RGV, RAS)",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_ORMO01", "desc":""                  },
   {"type":"ORDER_ORMO01",   "desc":""                   ,"req":true ,"repeats":true}]},
 "ORNO08":{
  "desc":"Stock requisition acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_ORNO08", "desc":""                      }]},
 "ORPO10":{
  "desc":"Pharmacy/treatment order acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_ORPO10", "desc":""                      }]},
 "ORRO02":{
  "desc":"Order response (also RRE, RRD, RRG, RRA)",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_ORRO02", "desc":""                      }]},
 "ORSO06":{
  "desc":"Stock requisition acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_ORSO06", "desc":""                      }]},
 "ORUR01":{
  "desc":"Unsolicited observation message",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"       ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"     ,"repeats":true},
   {"type":"PATIENT_RESULT", "desc":""                     ,"req":true ,"repeats":true},
   {"type":"DSC",            "desc":"Continuation Pointer"}]},
 "ORUR30":{
  "desc":"Of-care Observation Message Without Existing Order",
  "type":"message",
  "children":[
   {"type":"MSH",                "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",                "desc":"Software Segment"               ,"repeats":true},
   {"type":"PID",                "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                "desc":"Patient Additional Demographic"},
   {"type":"VISIT_ORUR30",       "desc":""                              },
   {"type":"ORC",                "desc":"Common Order"                   ,"req":true},
   {"type":"OBR",                "desc":"Observation Request"            ,"req":true},
   {"type":"NTE",                "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"TIMING_QTY_ORUR30",  "desc":""                               ,"repeats":true},
   {"type":"OBSERVATION_ORUR30", "desc":""                               ,"req":true ,"repeats":true}]},
 "ORUR31":{
  "desc":"Of-care Observation Message - Search For An Order",
  "type":"message",
  "children":[
   {"type":"MSH",                "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",                "desc":"Software Segment"               ,"repeats":true},
   {"type":"PID",                "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                "desc":"Patient Additional Demographic"},
   {"type":"VISIT_ORUR31",       "desc":""                              },
   {"type":"ORC",                "desc":"Common Order"                   ,"req":true},
   {"type":"OBR",                "desc":"Observation Request"            ,"req":true},
   {"type":"NTE",                "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"TIMING_QTY_ORUR31",  "desc":""                               ,"repeats":true},
   {"type":"OBSERVATION_ORUR31", "desc":""                               ,"req":true ,"repeats":true}]},
 "ORUR32":{
  "desc":"Ordered Point-Of-Care Observation",
  "type":"message",
  "children":[
   {"type":"MSH",                "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",                "desc":"Software Segment"               ,"repeats":true},
   {"type":"PID",                "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                "desc":"Patient Additional Demographic"},
   {"type":"VISIT_ORUR32",       "desc":""                              },
   {"type":"ORC",                "desc":"Common Order"                   ,"req":true},
   {"type":"OBR",                "desc":"Observation Request"            ,"req":true},
   {"type":"NTE",                "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"TIMING_QTY_ORUR32",  "desc":""                               ,"repeats":true},
   {"type":"OBSERVATION_ORUR32", "desc":""                               ,"req":true ,"repeats":true}]},
 "OSD":{
  "desc":"Order Sequence Definition",
  "type":"composite",
  "children":[
   {"type":"ID", "desc":"Sequence/results Flag"                 },
   {"type":"ST", "desc":"Placer Order Number: Entity Identifier"},
   {"type":"IS", "desc":"Placer Order Number: Namespace ID"     },
   {"type":"ST", "desc":"Filler Order Number: Entity Identifier"},
   {"type":"IS", "desc":"Filler Order Number: Namespace ID"     },
   {"type":"ST", "desc":"Sequence Condition Value"              },
   {"type":"NM", "desc":"Maximum Number Of Repeats"             },
   {"type":"ST", "desc":"Placer Order Number: Universal ID"     },
   {"type":"ID", "desc":"Placer Order Number: Universal ID Type"},
   {"type":"ST", "desc":"Filler Order Number: Universal ID"     },
   {"type":"ID", "desc":"Filler Order Number: Universal ID Type"}]},
 "OSP":{
  "desc":"Occurrence Span Code And Date",
  "type":"composite",
  "children":[
   {"type":"CNE", "desc":"Occurrence Span Code"      },
   {"type":"DT",  "desc":"Occurrence Span Start Date"},
   {"type":"DT",  "desc":"Occurrence Span Stop Date" }]},
 "OSQQ06":{
  "desc":"Query for order status",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"},
   {"type":"DSC", "desc":"Continuation Pointer"       }]},
 "OSRQ06":{
  "desc":"Query for order status - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"              ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment"      ,"req":true},
   {"type":"ERR",             "desc":"Error"                       ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"            ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"          ,"repeats":true},
   {"type":"QRD",             "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",             "desc":"Original Style Query Filter"},
   {"type":"RESPONSE_OSRQ06", "desc":""                           },
   {"type":"DSC",             "desc":"Continuation Pointer"       }]},
 "OULR21":{
  "desc":"Unsolicited laboratory observation",
  "type":"message",
  "children":[
   {"type":"MSH",                      "desc":"Message Header"       ,"req":true},
   {"type":"SFT",                      "desc":"Software Segment"     ,"repeats":true},
   {"type":"NTE",                      "desc":"Notes And Comments"  },
   {"type":"PATIENT_OULR21",           "desc":""                    },
   {"type":"VISIT_OULR21",             "desc":""                    },
   {"type":"ORDER_OBSERVATION_OULR21", "desc":""                     ,"req":true ,"repeats":true},
   {"type":"DSC",                      "desc":"Continuation Pointer"}]},
 "OULR22":{
  "desc":"Unsolicited Specimen Oriented Observation Message",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"       ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"     ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"  },
   {"type":"PATIENT_OULR22",  "desc":""                    },
   {"type":"VISIT_OULR22",    "desc":""                    },
   {"type":"SPECIMEN_OULR22", "desc":""                     ,"req":true ,"repeats":true},
   {"type":"DSC",             "desc":"Continuation Pointer"}]},
 "OULR23":{
  "desc":"Unsolicited Specimen Container Oriented Observation Message",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"       ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"     ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"  },
   {"type":"PATIENT_OULR23", "desc":""                    },
   {"type":"VISIT_OULR23",   "desc":""                    },
   {"type":"Group3_OULR23",  "desc":""                     ,"req":true ,"repeats":true},
   {"type":"DSC",            "desc":"Continuation Pointer"}]},
 "OULR24":{
  "desc":"Unsolicited Order Oriented Observation Message",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"       ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"     ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"  },
   {"type":"PATIENT_OULR24", "desc":""                    },
   {"type":"VISIT_OULR24",   "desc":""                    },
   {"type":"ORDER_OULR24",   "desc":""                     ,"req":true ,"repeats":true},
   {"type":"DSC",            "desc":"Continuation Pointer"}]},
 "OVR":{
  "desc":"Override Segment",
  "type":"segment",
  "children":[
   {"type":"CWE", "desc":"Business Rule Override Type"},
   {"type":"CWE", "desc":"Business Rule Override Code"},
   {"type":"TX",  "desc":"Override Comments"          },
   {"type":"XCN", "desc":"Override Entered By"        },
   {"type":"XCN", "desc":"Override Authorized By"     }]},
 "PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_GOAL_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",                          "desc":"Pathway"            ,"req":true},
   {"type":"NTE",                          "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                          "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE_PATHWAY_PATIENT", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PATHWAY_PATIENT",         "desc":""                   ,"repeats":true}]},
 "PATHWAY_PATIENT_PTRPCF":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",                                 "desc":"Pathway"            ,"req":true},
   {"type":"NTE",                                 "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                 "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE_PATHWAY_PATIENT_PTRPCF", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_PATHWAY_PATIENT",             "desc":""                   ,"repeats":true}]},
 "PATHWAY_PPGPCG":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",          "desc":"Pathway"            ,"req":true},
   {"type":"NTE",          "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",          "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PATHWAY", "desc":""                   ,"repeats":true}]},
 "PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",                  "desc":"Pathway"            ,"req":true},
   {"type":"NTE",                  "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                  "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE_PATHWAY", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PATHWAY_PPGPCH",  "desc":""                   ,"repeats":true}]},
 "PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",                         "desc":"Pathway"            ,"req":true},
   {"type":"NTE",                         "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                         "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE_PATHWAY_PPGPCJ", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PATHWAY_PPGPCJ",         "desc":""                   ,"repeats":true}]},
 "PATHWAY_PPPPCB":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",                         "desc":"Pathway"            ,"req":true},
   {"type":"NTE",                         "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                         "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE_PATHWAY_PPPPCB", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_PATHWAY",             "desc":""                   ,"repeats":true}]},
 "PATHWAY_PPPPCC":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",                         "desc":"Pathway"            ,"req":true},
   {"type":"NTE",                         "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                         "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE_PATHWAY_PPPPCC", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_PATHWAY_PPPPCC",      "desc":""                   ,"repeats":true}]},
 "PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH",                         "desc":"Pathway"            ,"req":true},
   {"type":"NTE",                         "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                         "desc":"Variance"           ,"repeats":true},
   {"type":"PATHWAY_ROLE_PATHWAY_PPPPCD", "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_PATHWAY_PPPPCD",      "desc":""                   ,"repeats":true}]},
 "PATHWAY_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE_PATHWAY_PATIENT_PTRPCF":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE_PATHWAY_PPPPCB":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE_PATHWAY_PPPPCC":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATHWAY_ROLE_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                 }]},
 "PATIENT_ADTA40":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                 }]},
 "PATIENT_ADTA41":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                 }]},
 "PATIENT_ADTA42":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                 }]},
 "PATIENT_ADTA43":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "PATIENT_ADTA44":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"MRG", "desc":"Merge Patient Information"      ,"req":true}]},
 "PATIENT_BARP02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"DB1", "desc":"Disability"                     ,"repeats":true}]},
 "PATIENT_BARP06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         }]},
 "PATIENT_BPSO29":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",           "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",           "desc":"Patient Additional Demographic"},
   {"type":"NTE",           "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT", "desc":""                              }]},
 "PATIENT_BRPO30":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",           "desc":"Patient Identification" ,"req":true},
   {"type":"ORDER_PATIENT", "desc":""                       ,"repeats":true}]},
 "PATIENT_BTSO31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                   "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                   "desc":"Patient Additional Demographic"},
   {"type":"NTE",                   "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT", "desc":""                              }]},
 "PATIENT_CRMC01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CRMC02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CRMC03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CRMC04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CRMC05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CRMC06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CRMC07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CRMC08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"      ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"              },
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "PATIENT_CSUC09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",           "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",           "desc":"Patient Additional Demographic"},
   {"type":"NTE",           "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"VISIT_PATIENT", "desc":""                              },
   {"type":"CSR",           "desc":"Clinical Study Registration"    ,"req":true},
   {"type":"STUDY_PHASE",   "desc":""                               ,"req":true ,"repeats":true}]},
 "PATIENT_CSUC10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                  "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                  "desc":"Patient Additional Demographic"},
   {"type":"NTE",                  "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"VISIT_PATIENT_CSUC10", "desc":""                              },
   {"type":"CSR",                  "desc":"Clinical Study Registration"    ,"req":true},
   {"type":"STUDY_PHASE_PATIENT",  "desc":""                               ,"req":true ,"repeats":true}]},
 "PATIENT_CSUC11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                        "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                        "desc":"Patient Additional Demographic"},
   {"type":"NTE",                        "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"VISIT_PATIENT_CSUC11",       "desc":""                              },
   {"type":"CSR",                        "desc":"Clinical Study Registration"    ,"req":true},
   {"type":"STUDY_PHASE_PATIENT_CSUC11", "desc":""                               ,"req":true ,"repeats":true}]},
 "PATIENT_CSUC12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                        "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                        "desc":"Patient Additional Demographic"},
   {"type":"NTE",                        "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"VISIT_PATIENT_CSUC12",       "desc":""                              },
   {"type":"CSR",                        "desc":"Clinical Study Registration"    ,"req":true},
   {"type":"STUDY_PHASE_PATIENT_CSUC12", "desc":""                               ,"req":true ,"repeats":true}]},
 "PATIENT_DEFINITION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_DEFINITION_RDRRDR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_DEFINITION_RERRER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_DEFINITION_RGRRGR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_DEFINITION_RORROR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_OMBO27":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMBO27", "desc":""                              },
   {"type":"INSURANCE_PATIENT",            "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_OMDO03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMDO03", "desc":""                              },
   {"type":"INSURANCE_PATIENT_OMDO03",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_OMGO19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"  },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMGO19", "desc":""                                },
   {"type":"INSURANCE_PATIENT_OMGO19",     "desc":""                                 ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                       },
   {"type":"AL1",                          "desc":"Patient Allergy Information"      ,"repeats":true}]},
 "PATIENT_OMIO23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMIO23", "desc":""                              },
   {"type":"INSURANCE_PATIENT_OMIO23",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_OMLO21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"  },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMLO21", "desc":""                                },
   {"type":"INSURANCE_PATIENT_OMLO21",     "desc":""                                 ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                       },
   {"type":"AL1",                          "desc":"Patient Allergy Information"      ,"repeats":true}]},
 "PATIENT_OMLO33":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"  },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMLO33", "desc":""                                },
   {"type":"INSURANCE_PATIENT_OMLO33",     "desc":""                                 ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                       },
   {"type":"AL1",                          "desc":"Patient Allergy Information"      ,"repeats":true}]},
 "PATIENT_OMLO35":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"  },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMLO35", "desc":""                                },
   {"type":"INSURANCE_PATIENT_OMLO35",     "desc":""                                 ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                       },
   {"type":"AL1",                          "desc":"Patient Allergy Information"      ,"repeats":true}]},
 "PATIENT_OMNO07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMNO07", "desc":""                              },
   {"type":"INSURANCE_PATIENT_OMNO07",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_OMPO09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMPO09", "desc":""                              },
   {"type":"INSURANCE_PATIENT_OMPO09",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_OMSO05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_OMSO05", "desc":""                              },
   {"type":"INSURANCE_PATIENT_OMSO05",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_ORBO28":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"ORDER_PATIENT_ORBO28", "desc":""                       ,"repeats":true}]},
 "PATIENT_ORLO22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"ORDER_PATIENT_ORLO22", "desc":""                       ,"repeats":true}]},
 "PATIENT_ORLO34":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",              "desc":"Patient Identification" ,"req":true},
   {"type":"SPECIMEN_PATIENT", "desc":""                       ,"req":true ,"repeats":true}]},
 "PATIENT_ORLO36":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                     "desc":"Patient Identification" ,"req":true},
   {"type":"SPECIMEN_PATIENT_ORLO36", "desc":""                       ,"req":true ,"repeats":true}]},
 "PATIENT_ORMO01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_ORMO01", "desc":""                              },
   {"type":"INSURANCE_PATIENT_ORMO01",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_OULR21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"NTE", "desc":"Notes And Comments"             ,"repeats":true}]},
 "PATIENT_OULR22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"NTE", "desc":"Notes And Comments"             ,"repeats":true}]},
 "PATIENT_OULR23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"NTE", "desc":"Notes And Comments"             ,"repeats":true}]},
 "PATIENT_OULR24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"NTE", "desc":"Notes And Comments"             ,"repeats":true}]},
 "PATIENT_PATIENT_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"  },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"VISIT_PATIENT_PATIENT_RESULT", "desc":""                                }]},
 "PATIENT_PPTPCL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PATIENT_PPTPCL", "desc":""                      },
   {"type":"PATHWAY_PATIENT",              "desc":""                       ,"req":true ,"repeats":true}]},
 "PATIENT_PPVPCA":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PATIENT_PPVPCA", "desc":""                      },
   {"type":"GOAL_PATIENT",                 "desc":""                       ,"req":true ,"repeats":true}]},
 "PATIENT_PRIOR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"}]},
 "PATIENT_PRIOR_PRIOR_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"}]},
 "PATIENT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"}]},
 "PATIENT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"}]},
 "PATIENT_PRRPC5":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PATIENT_PRRPC5", "desc":""                      },
   {"type":"PROBLEM_PATIENT",              "desc":""                       ,"req":true ,"repeats":true}]},
 "PATIENT_PTRPCF":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PATIENT_PTRPCF", "desc":""                      },
   {"type":"PATHWAY_PATIENT_PTRPCF",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PATIENT_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_QUERY_RESPONSE_RSPZ82":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"VISIT_PATIENT_QUERY_RESPONSE", "desc":""                              }]},
 "PATIENT_QUERY_RESPONSE_RSPZ86":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"NTE", "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"AL1", "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_QUERY_RESPONSE_RSPZ88":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",     "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",     "desc":"Patient Additional Demographic"},
   {"type":"NTE",     "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"ALLERGY", "desc":""                              }]},
 "PATIENT_QUERY_RESPONSE_RSPZ90":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                                 "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",                                 "desc":"Patient Additional Demographic"  },
   {"type":"NK1",                                 "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"NTE",                                 "desc":"Notes And Comments"               ,"repeats":true},
   {"type":"VISIT_PATIENT_QUERY_RESPONSE_RSPZ90", "desc":""                                }]},
 "PATIENT_RASO17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_RASO17", "desc":""                              }]},
 "PATIENT_RDEO11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_RDEO11", "desc":""                              },
   {"type":"INSURANCE_PATIENT_RDEO11",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_RDEO25", "desc":""                              },
   {"type":"INSURANCE_PATIENT_RDEO25",     "desc":""                               ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                     },
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true}]},
 "PATIENT_RDSO13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                          "desc":"Patient Additional Demographic"},
   {"type":"NTE",                          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information"    ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_RDSO13", "desc":""                              }]},
 "PATIENT_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_ORGO20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_ORIO24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_ORNO08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_ORPO10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_ORRO02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_ORSO06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_OSRQ06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_RRAO18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_RRDO14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_RREO12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_RREO26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_RRGO16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"     ,"repeats":true}]},
 "PATIENT_RESPONSE_RSPK31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                            "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",                            "desc":"Patient Additional Demographic"},
   {"type":"NTE",                            "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"AL1",                            "desc":"Patient Allergy Information"    ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_RESPONSE", "desc":""                              }]},
 "PATIENT_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_PATIENT_RESULT", "desc":""},
   {"type":"ORDER_OBSERVATION",      "desc":"" ,"req":true ,"repeats":true}]},
 "PATIENT_RGVO15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",                          "desc":"Patient Identification"      ,"req":true},
   {"type":"NTE",                          "desc":"Notes And Comments"          ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"PATIENT_VISIT_PATIENT_RGVO15", "desc":""                           }]},
 "PATIENT_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"             }]},
 "PATIENT_SCHEDULE_SRRS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SCHEDULE_SRRS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SIUS12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SIUS26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"},
   {"type":"PV1", "desc":"Patient Visit"                 },
   {"type":"PV2", "desc":"Additional Information"        },
   {"type":"OBX", "desc":"Observation/Result"             ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"                      ,"repeats":true}]},
 "PATIENT_SRMS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_SRMS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification" ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"         },
   {"type":"PV2", "desc":"Additional Information"},
   {"type":"OBX", "desc":"Observation/Result"     ,"repeats":true},
   {"type":"DG1", "desc":"Diagnosis"              ,"repeats":true}]},
 "PATIENT_VISIT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMBO27":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMDO03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMGO19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMIO23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMLO21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMLO33":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMLO35":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMNO07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMPO09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_OMSO05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_ORMO01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_PPTPCL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_PPVPCA":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_PRRPC5":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_PTRPCF":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_RASO17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_RDEO11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_RDSO13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PATIENT_RGVO15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PGLPC6":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PGLPC7":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPGPCG":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPPPCB":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPPPCC":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PRIOR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PRIOR_PRIOR_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_REFI12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_REFI13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_RRII12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_RRII13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_RRII14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_RRII15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VISIT_VXRV03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VXUV04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "PATIENT_VXXV02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1", "desc":"Next Of Kin / Associated Parties" ,"repeats":true}]},
 "PCR":{
  "desc":"Possible Causal Relationship",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Implicated Product"                    ,"req":true},
   {"type":"IS", "desc":"Generic Product"                      },
   {"type":"CE", "desc":"Product Class"                        },
   {"type":"CQ", "desc":"Total Duration Of Therapy"            },
   {"type":"TS", "desc":"Product Manufacture Date"             },
   {"type":"TS", "desc":"Product Expiration Date"              },
   {"type":"TS", "desc":"Product Implantation Date"            },
   {"type":"TS", "desc":"Product Explantation Date"            },
   {"type":"IS", "desc":"Single Use Device"                    },
   {"type":"CE", "desc":"Indication For Product Use"           },
   {"type":"IS", "desc":"Product Problem"                      },
   {"type":"ST", "desc":"Product Serial/lot Number"             ,"repeats":true},
   {"type":"IS", "desc":"Product Available For Inspection"     },
   {"type":"CE", "desc":"Product Evaluation Performed"         },
   {"type":"CE", "desc":"Product Evaluation Status"            },
   {"type":"CE", "desc":"Product Evaluation Results"           },
   {"type":"ID", "desc":"Evaluated Product Source"             },
   {"type":"TS", "desc":"Date Product Returned To Manufacturer"},
   {"type":"ID", "desc":"Device Operator Qualifications"       },
   {"type":"ID", "desc":"Relatedness Assessment"               },
   {"type":"ID", "desc":"Action Taken In Response To The Event" ,"repeats":true},
   {"type":"ID", "desc":"Event Causality Observations"          ,"repeats":true},
   {"type":"ID", "desc":"Indirect Exposure Mechanism"           ,"repeats":true}]},
 "PD1":{
  "desc":"Patient Additional Demographic",
  "type":"segment",
  "children":[
   {"type":"IS",  "desc":"Living Dependency"                           ,"repeats":true},
   {"type":"IS",  "desc":"Living Arrangement"                         },
   {"type":"XON", "desc":"Patient Primary Facility"                    ,"repeats":true},
   {"type":"XCN", "desc":"Patient Primary Care Provider Name & ID No." ,"repeats":true},
   {"type":"IS",  "desc":"Student Indicator"                          },
   {"type":"IS",  "desc":"Handicap"                                   },
   {"type":"IS",  "desc":"Living Will Code"                           },
   {"type":"IS",  "desc":"Organ Donor Code"                           },
   {"type":"ID",  "desc":"Separate Bill"                              },
   {"type":"CX",  "desc":"Duplicate Patient"                           ,"repeats":true},
   {"type":"CE",  "desc":"Publicity Code"                             },
   {"type":"ID",  "desc":"Protection Indicator"                       },
   {"type":"DT",  "desc":"Protection Indicator Effective Date"        },
   {"type":"XON", "desc":"Place Of Worship"                            ,"repeats":true},
   {"type":"CE",  "desc":"Advance Directive Code"                      ,"repeats":true},
   {"type":"IS",  "desc":"Immunization Registry Status"               },
   {"type":"DT",  "desc":"Immunization Registry Status Effective Date"},
   {"type":"DT",  "desc":"Publicity Code Effective Date"              },
   {"type":"IS",  "desc":"Military Branch"                            },
   {"type":"IS",  "desc":"Military Rank/grade"                        },
   {"type":"IS",  "desc":"Military Status"                            }]},
 "PDA":{
  "desc":"Patient Death And Autopsy",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Death Cause Code"                   ,"repeats":true},
   {"type":"PL",  "desc":"Death Location"                    },
   {"type":"ID",  "desc":"Death Certified Indicator"         },
   {"type":"TS",  "desc":"Death Certificate Signed Date/time"},
   {"type":"XCN", "desc":"Death Certified By"                },
   {"type":"ID",  "desc":"Autopsy Indicator"                 },
   {"type":"DR",  "desc":"Autopsy Start And End Date/time"   },
   {"type":"XCN", "desc":"Autopsy Performed By"              },
   {"type":"ID",  "desc":"Coroner Indicator"                 }]},
 "PDC":{
  "desc":"Product Detail Country",
  "type":"segment",
  "children":[
   {"type":"XON", "desc":"Manufacturer/distributor" ,"req":true ,"repeats":true},
   {"type":"CE",  "desc":"Country"                  ,"req":true},
   {"type":"ST",  "desc":"Brand Name"               ,"req":true},
   {"type":"ST",  "desc":"Device Family Name"      },
   {"type":"CE",  "desc":"Generic Name"            },
   {"type":"ST",  "desc":"Model Identifier"         ,"repeats":true},
   {"type":"ST",  "desc":"Catalogue Identifier"    },
   {"type":"ST",  "desc":"Other Identifier"         ,"repeats":true},
   {"type":"CE",  "desc":"Product Code"            },
   {"type":"ID",  "desc":"Marketing Basis"         },
   {"type":"ST",  "desc":"Marketing Approval ID"   },
   {"type":"CQ",  "desc":"Labeled Shelf Life"      },
   {"type":"CQ",  "desc":"Expected Shelf Life"     },
   {"type":"TS",  "desc":"Date First Marketed"     },
   {"type":"TS",  "desc":"Date Last Marketed"      }]},
 "PEO":{
  "desc":"Product Experience Observation",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Event Identifiers Used"                      ,"repeats":true},
   {"type":"CE",  "desc":"Event Symptom/diagnosis Code"                ,"repeats":true},
   {"type":"TS",  "desc":"Event Onset Date/time"                       ,"req":true},
   {"type":"TS",  "desc":"Event Exacerbation Date/time"               },
   {"type":"TS",  "desc":"Event Improved Date/time"                   },
   {"type":"TS",  "desc":"Event Ended Data/time"                      },
   {"type":"XAD", "desc":"Event Location Occurred Address"             ,"repeats":true},
   {"type":"ID",  "desc":"Event Qualification"                         ,"repeats":true},
   {"type":"ID",  "desc":"Event Serious"                              },
   {"type":"ID",  "desc":"Event Expected"                             },
   {"type":"ID",  "desc":"Event Outcome"                               ,"repeats":true},
   {"type":"ID",  "desc":"Patient Outcome"                            },
   {"type":"FT",  "desc":"Event Description From Others"               ,"repeats":true},
   {"type":"FT",  "desc":"Event From Original Reporter"                ,"repeats":true},
   {"type":"FT",  "desc":"Event Description From Patient"              ,"repeats":true},
   {"type":"FT",  "desc":"Event Description From Practitioner"         ,"repeats":true},
   {"type":"FT",  "desc":"Event Description From Autopsy"              ,"repeats":true},
   {"type":"CE",  "desc":"Cause Of Death"                              ,"repeats":true},
   {"type":"XPN", "desc":"Primary Observer Name"                       ,"repeats":true},
   {"type":"XAD", "desc":"Primary Observer Address"                    ,"repeats":true},
   {"type":"XTN", "desc":"Primary Observer Telephone"                  ,"repeats":true},
   {"type":"ID",  "desc":"Primary Observer's Qualification"           },
   {"type":"ID",  "desc":"Confirmation Provided By"                   },
   {"type":"TS",  "desc":"Primary Observer Aware Date/time"           },
   {"type":"ID",  "desc":"Primary Observer's Identity May Be Divulged"}]},
 "PERSONNEL_RESOURCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     }]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SIUS26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PERSONNEL_RESOURCE_RESOURCES_SRMS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIP", "desc":"Appointment Information - Personnel Resource" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"                     },
   {"type":"NTE", "desc":"Notes And Comments"                           ,"repeats":true}]},
 "PES":{
  "desc":"Product Experience Sender",
  "type":"segment",
  "children":[
   {"type":"XON", "desc":"Sender Organization Name" ,"repeats":true},
   {"type":"XCN", "desc":"Sender Individual Name"   ,"repeats":true},
   {"type":"XAD", "desc":"Sender Address"           ,"repeats":true},
   {"type":"XTN", "desc":"Sender Telephone"         ,"repeats":true},
   {"type":"EI",  "desc":"Sender Event Identifier" },
   {"type":"NM",  "desc":"Sender Sequence Number"  },
   {"type":"FT",  "desc":"Sender Event Description" ,"repeats":true},
   {"type":"FT",  "desc":"Sender Comment"          },
   {"type":"TS",  "desc":"Sender Aware Date/time"  },
   {"type":"TS",  "desc":"Event Report Date"        ,"req":true},
   {"type":"ID",  "desc":"Event Report Timing/type" ,"repeats":true},
   {"type":"ID",  "desc":"Event Report Source"     },
   {"type":"ID",  "desc":"Event Reported To"        ,"repeats":true}]},
 "PEXP07":{
  "desc":"Unsolicited initial individual product experience request",
  "type":"message",
  "children":[
   {"type":"MSH",          "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",          "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN",          "desc":"Event Type"                     ,"req":true},
   {"type":"PID",          "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",          "desc":"Patient Additional Demographic"},
   {"type":"NTE",          "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"VISIT_PEXP07", "desc":""                              },
   {"type":"EXPERIENCE",   "desc":""                               ,"req":true ,"repeats":true}]},
 "PEXP08":{
  "desc":"Unsolicited update individual product experience report",
  "type":"message",
  "children":[
   {"type":"MSH",               "desc":"Message Header"                 ,"req":true},
   {"type":"SFT",               "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN",               "desc":"Event Type"                     ,"req":true},
   {"type":"PID",               "desc":"Patient Identification"         ,"req":true},
   {"type":"PD1",               "desc":"Patient Additional Demographic"},
   {"type":"NTE",               "desc":"Notes And Comments"             ,"repeats":true},
   {"type":"VISIT_PEXP08",      "desc":""                              },
   {"type":"EXPERIENCE_PEXP08", "desc":""                               ,"req":true ,"repeats":true}]},
 "PEX_CAUSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PCR",               "desc":"Possible Causal Relationship" ,"req":true},
   {"type":"RX_ORDER",          "desc":""                            },
   {"type":"RX_ADMINISTRATION", "desc":""                             ,"repeats":true},
   {"type":"PRB",               "desc":"Problem Details"              ,"repeats":true},
   {"type":"OBX",               "desc":"Observation/Result"           ,"repeats":true},
   {"type":"NTE",               "desc":"Notes And Comments"           ,"repeats":true},
   {"type":"ASSOCIATED_PERSON", "desc":""                            },
   {"type":"STUDY",             "desc":""                             ,"repeats":true}]},
 "PEX_CAUSE_PEX_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PCR",                         "desc":"Possible Causal Relationship" ,"req":true},
   {"type":"RX_ORDER_PEX_CAUSE",          "desc":""                            },
   {"type":"RX_ADMINISTRATION_PEX_CAUSE", "desc":""                             ,"repeats":true},
   {"type":"PRB",                         "desc":"Problem Details"              ,"repeats":true},
   {"type":"OBX",                         "desc":"Observation/Result"           ,"repeats":true},
   {"type":"NTE",                         "desc":"Notes And Comments"           ,"repeats":true},
   {"type":"ASSOCIATED_PERSON_PEX_CAUSE", "desc":""                            },
   {"type":"STUDY_PEX_CAUSE",             "desc":""                             ,"repeats":true}]},
 "PEX_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PEO",       "desc":"Product Experience Observation" ,"req":true},
   {"type":"PEX_CAUSE", "desc":""                               ,"req":true ,"repeats":true}]},
 "PEX_OBSERVATION_EXPERIENCE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PEO",                       "desc":"Product Experience Observation" ,"req":true},
   {"type":"PEX_CAUSE_PEX_OBSERVATION", "desc":""                               ,"req":true ,"repeats":true}]},
 "PGLPC6":{
  "desc":"Patient goal add",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PGLPC6", "desc":""                      },
   {"type":"GOAL",                 "desc":""                       ,"req":true ,"repeats":true}]},
 "PGLPC7":{
  "desc":"Patient goal update",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PGLPC7", "desc":""                      },
   {"type":"GOAL_PGLPC7",          "desc":""                       ,"req":true ,"repeats":true}]},
 "PGLPC8":{
  "desc":"Patient goal delete",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PGLPC8", "desc":""                      },
   {"type":"GOAL_PGLPC8",          "desc":""                       ,"req":true ,"repeats":true}]},
 "PID":{
  "desc":"Patient Identification",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Pid"                     },
   {"type":"CX",  "desc":"Patient ID"                       },
   {"type":"CX",  "desc":"Patient Identifier List"           ,"req":true ,"repeats":true},
   {"type":"CX",  "desc":"Alternate Patient ID - Pid"        ,"repeats":true},
   {"type":"XPN", "desc":"Patient Name"                      ,"req":true ,"repeats":true},
   {"type":"XPN", "desc":"Mother's Maiden Name"              ,"repeats":true},
   {"type":"TS",  "desc":"Date/time Of Birth"               },
   {"type":"IS",  "desc":"Administrative Sex"               },
   {"type":"XPN", "desc":"Patient Alias"                     ,"repeats":true},
   {"type":"CE",  "desc":"Race"                              ,"repeats":true},
   {"type":"XAD", "desc":"Patient Address"                   ,"repeats":true},
   {"type":"IS",  "desc":"County Code"                      },
   {"type":"XTN", "desc":"Phone Number - Home"               ,"repeats":true},
   {"type":"XTN", "desc":"Phone Number - Business"           ,"repeats":true},
   {"type":"CE",  "desc":"Primary Language"                 },
   {"type":"CE",  "desc":"Marital Status"                   },
   {"type":"CE",  "desc":"Religion"                         },
   {"type":"CX",  "desc":"Patient Account Number"           },
   {"type":"ST",  "desc":"Ssn Number - Patient"             },
   {"type":"DLN", "desc":"Driver's License Number - Patient"},
   {"type":"CX",  "desc":"Mother's Identifier"               ,"repeats":true},
   {"type":"CE",  "desc":"Ethnic Group"                      ,"repeats":true},
   {"type":"ST",  "desc":"Birth Place"                      },
   {"type":"ID",  "desc":"Multiple Birth Indicator"         },
   {"type":"NM",  "desc":"Birth Order"                      },
   {"type":"CE",  "desc":"Citizenship"                       ,"repeats":true},
   {"type":"CE",  "desc":"Veterans Military Status"         },
   {"type":"CE",  "desc":"Nationality"                      },
   {"type":"TS",  "desc":"Patient Death Date And Time"      },
   {"type":"ID",  "desc":"Patient Death Indicator"          },
   {"type":"ID",  "desc":"Identity Unknown Indicator"       },
   {"type":"IS",  "desc":"Identity Reliability Code"         ,"repeats":true},
   {"type":"TS",  "desc":"Last Update Date/time"            },
   {"type":"HD",  "desc":"Last Update Facility"             },
   {"type":"CE",  "desc":"Species Code"                     },
   {"type":"CE",  "desc":"Breed Code"                       },
   {"type":"ST",  "desc":"Strain"                           },
   {"type":"CE",  "desc":"Production Class Code"            },
   {"type":"CWE", "desc":"Tribal Citizenship"                ,"repeats":true}]},
 "PIP":{
  "desc":"Practitioner Institutional Privileges",
  "type":"composite",
  "children":[
   {"type":"CE", "desc":"Privilege"      },
   {"type":"CE", "desc":"Privilege Class"},
   {"type":"DT", "desc":"Expiration Date"},
   {"type":"DT", "desc":"Activation Date"},
   {"type":"EI", "desc":"Facility"       }]},
 "PL":{
  "desc":"Person Location",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Point Of Care"                    },
   {"type":"IS", "desc":"Room"                             },
   {"type":"IS", "desc":"Bed"                              },
   {"type":"HD", "desc":"Facility"                         },
   {"type":"IS", "desc":"Location Status"                  },
   {"type":"IS", "desc":"Person Location Type"             },
   {"type":"IS", "desc":"Building"                         },
   {"type":"IS", "desc":"Floor"                            },
   {"type":"ST", "desc":"Location Description"             },
   {"type":"EI", "desc":"Comprehensive Location Identifier"},
   {"type":"HD", "desc":"Assigning Authority For Location" }]},
 "PLN":{
  "desc":"Practitioner License Or Other ID Number",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"ID Number"                         },
   {"type":"IS", "desc":"Type Of ID Number"                 },
   {"type":"ST", "desc":"State/other Qualifying Information"},
   {"type":"DT", "desc":"Expiration Date"                   }]},
 "PMUB01":{
  "desc":"Add personnel record",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"STF", "desc":"Staff Identification"           ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail"            ,"repeats":true},
   {"type":"ORG", "desc":"Practitioner Organization Unit" ,"repeats":true},
   {"type":"AFF", "desc":"Professional Affiliation"       ,"repeats":true},
   {"type":"LAN", "desc":"Language Detail"                ,"repeats":true},
   {"type":"EDU", "desc":"Educational Detail"             ,"repeats":true},
   {"type":"CER", "desc":"Certificate Detail"             ,"repeats":true}]},
 "PMUB02":{
  "desc":"Update personnel record",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"STF", "desc":"Staff Identification"           ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail"            ,"repeats":true},
   {"type":"ORG", "desc":"Practitioner Organization Unit" ,"repeats":true},
   {"type":"AFF", "desc":"Professional Affiliation"       ,"repeats":true},
   {"type":"LAN", "desc":"Language Detail"                ,"repeats":true},
   {"type":"EDU", "desc":"Educational Detail"             ,"repeats":true},
   {"type":"CER", "desc":"Certificate Detail"             ,"repeats":true}]},
 "PMUB03":{
  "desc":"Delete personnel record",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"       ,"req":true},
   {"type":"SFT", "desc":"Software Segment"     ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"           ,"req":true},
   {"type":"STF", "desc":"Staff Identification" ,"req":true}]},
 "PMUB04":{
  "desc":"Active practicing person",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"STF", "desc":"Staff Identification"           ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail"            ,"repeats":true},
   {"type":"ORG", "desc":"Practitioner Organization Unit" ,"repeats":true}]},
 "PMUB05":{
  "desc":"Deactivate practicing person",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"STF", "desc":"Staff Identification"           ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail"            ,"repeats":true},
   {"type":"ORG", "desc":"Practitioner Organization Unit" ,"repeats":true}]},
 "PMUB06":{
  "desc":"Terminate practicing person",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"                 ,"req":true},
   {"type":"SFT", "desc":"Software Segment"               ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"                     ,"req":true},
   {"type":"STF", "desc":"Staff Identification"           ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail"            ,"repeats":true},
   {"type":"ORG", "desc":"Practitioner Organization Unit" ,"repeats":true}]},
 "PMUB07":{
  "desc":"Grant Certificate/Permission",
  "type":"message",
  "children":[
   {"type":"MSH",    "desc":"Message Header"       ,"req":true},
   {"type":"SFT",    "desc":"Software Segment"     ,"repeats":true},
   {"type":"EVN",    "desc":"Event Type"           ,"req":true},
   {"type":"STF",    "desc":"Staff Identification" ,"req":true},
   {"type":"PRA",    "desc":"Practitioner Detail" },
   {"type":"Group1", "desc":""                     ,"repeats":true}]},
 "PMUB08":{
  "desc":"Revoke Certificate/Permission",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"       ,"req":true},
   {"type":"SFT", "desc":"Software Segment"     ,"repeats":true},
   {"type":"EVN", "desc":"Event Type"           ,"req":true},
   {"type":"STF", "desc":"Staff Identification" ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail" },
   {"type":"CER", "desc":"Certificate Detail"   ,"repeats":true}]},
 "PPGPCG":{
  "desc":"Patient pathway (goal-oriented) add",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPGPCG", "desc":""                      },
   {"type":"PATHWAY_PPGPCG",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPGPCH":{
  "desc":"Patient pathway (goal-oriented) update",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPGPCH", "desc":""                      },
   {"type":"PATHWAY_PPGPCH",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPGPCJ":{
  "desc":"Patient pathway (goal-oriented) delete",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPGPCJ", "desc":""                      },
   {"type":"PATHWAY_PPGPCJ",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPN":{
  "desc":"Performing Person Time Stamp",
  "type":"composite",
  "children":[
   {"type":"ST",  "desc":"ID Number"                                         },
   {"type":"FN",  "desc":"Family Name"                                       },
   {"type":"ST",  "desc":"Given Name"                                        },
   {"type":"ST",  "desc":"Second And Further Given Names Or Initials Thereof"},
   {"type":"ST",  "desc":"Suffix (e.g., Jr Or III)"                          },
   {"type":"ST",  "desc":"Prefix (e.g., Dr)"                                 },
   {"type":"IS",  "desc":"Degree (e.g., Md)"                                 },
   {"type":"IS",  "desc":"Source Table"                                      },
   {"type":"HD",  "desc":"Assigning Authority"                               },
   {"type":"ID",  "desc":"Name Type Code"                                    },
   {"type":"ST",  "desc":"Identifier Check Digit"                            },
   {"type":"ID",  "desc":"Check Digit Scheme"                                },
   {"type":"ID",  "desc":"Identifier Type Code"                              },
   {"type":"HD",  "desc":"Assigning Facility"                                },
   {"type":"TS",  "desc":"Date/time Action Performed"                        },
   {"type":"ID",  "desc":"Name Representation Code"                          },
   {"type":"CE",  "desc":"Name Context"                                      },
   {"type":"DR",  "desc":"Name Validity Range"                               },
   {"type":"ID",  "desc":"Name Assembly Order"                               },
   {"type":"TS",  "desc":"Effective Date"                                    },
   {"type":"TS",  "desc":"Expiration Date"                                   },
   {"type":"ST",  "desc":"Professional Suffix"                               },
   {"type":"CWE", "desc":"Assigning Jurisdiction"                            },
   {"type":"CWE", "desc":"Assigning Agency Or Department"                    }]},
 "PPPPCB":{
  "desc":"Patient pathway (problem-oriented) add",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPPPCB", "desc":""                      },
   {"type":"PATHWAY_PPPPCB",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPPPCC":{
  "desc":"Patient pathway (problem-oriented) update",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPPPCC", "desc":""                      },
   {"type":"PATHWAY_PPPPCC",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPPPCD":{
  "desc":"Patient pathway (problem-oriented) delete",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPPPCD", "desc":""                      },
   {"type":"PATHWAY_PPPPCD",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPRPC1":{
  "desc":"Patient problem add",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPRPC1", "desc":""                      },
   {"type":"PROBLEM_PPRPC1",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPRPC2":{
  "desc":"Patient problem update",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPRPC2", "desc":""                      },
   {"type":"PROBLEM_PPRPC2",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPRPC3":{
  "desc":"Patient problem delete",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"         ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"       ,"repeats":true},
   {"type":"PID",                  "desc":"Patient Identification" ,"req":true},
   {"type":"PATIENT_VISIT_PPRPC3", "desc":""                      },
   {"type":"PROBLEM_PPRPC3",       "desc":""                       ,"req":true ,"repeats":true}]},
 "PPTPCL":{
  "desc":"Patient pathway (goal-oriented) query response",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"QAK",            "desc":"Query Acknowledgment"  },
   {"type":"QRD",            "desc":"Style Query Definition" ,"req":true},
   {"type":"PATIENT_PPTPCL", "desc":""                       ,"req":true ,"repeats":true}]},
 "PPVPCA":{
  "desc":"Patient goal response",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"QAK",            "desc":"Query Acknowledgment"  },
   {"type":"QRD",            "desc":"Style Query Definition" ,"req":true},
   {"type":"PATIENT_PPVPCA", "desc":""                       ,"req":true ,"repeats":true}]},
 "PR1":{
  "desc":"Procedures",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Pr1"              ,"req":true},
   {"type":"IS",  "desc":"Procedure Coding Method"  },
   {"type":"CE",  "desc":"Procedure Code"            ,"req":true},
   {"type":"ST",  "desc":"Procedure Description"    },
   {"type":"TS",  "desc":"Procedure Date/time"       ,"req":true},
   {"type":"IS",  "desc":"Procedure Functional Type"},
   {"type":"NM",  "desc":"Procedure Minutes"        },
   {"type":"XCN", "desc":"Anesthesiologist"          ,"repeats":true},
   {"type":"IS",  "desc":"Anesthesia Code"          },
   {"type":"NM",  "desc":"Anesthesia Minutes"       },
   {"type":"XCN", "desc":"Surgeon"                   ,"repeats":true},
   {"type":"XCN", "desc":"Procedure Practitioner"    ,"repeats":true},
   {"type":"CE",  "desc":"Consent Code"             },
   {"type":"ID",  "desc":"Procedure Priority"       },
   {"type":"CE",  "desc":"Associated Diagnosis Code"},
   {"type":"CE",  "desc":"Procedure Code Modifier"   ,"repeats":true},
   {"type":"IS",  "desc":"Procedure Drg Type"       },
   {"type":"CE",  "desc":"Tissue Type Code"          ,"repeats":true},
   {"type":"EI",  "desc":"Procedure Identifier"     },
   {"type":"ID",  "desc":"Procedure Action Code"    }]},
 "PRA":{
  "desc":"Practitioner Detail",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Primary Key Value - Pra"                     },
   {"type":"CE",  "desc":"Practitioner Group"                           ,"repeats":true},
   {"type":"IS",  "desc":"Practitioner Category"                        ,"repeats":true},
   {"type":"ID",  "desc":"Provider Billing"                            },
   {"type":"SPD", "desc":"Specialty"                                    ,"repeats":true},
   {"type":"PLN", "desc":"Practitioner ID Numbers"                      ,"repeats":true},
   {"type":"PIP", "desc":"Privileges"                                   ,"repeats":true},
   {"type":"DT",  "desc":"Date Entered Practice"                       },
   {"type":"CE",  "desc":"Institution"                                 },
   {"type":"DT",  "desc":"Date Left Practice"                          },
   {"type":"CE",  "desc":"Government Reimbursement Billing Eligibility" ,"repeats":true},
   {"type":"SI",  "desc":"Set ID - Pra"                                }]},
 "PRB":{
  "desc":"Problem Details",
  "type":"segment",
  "children":[
   {"type":"ID", "desc":"Action Code"                                             ,"req":true},
   {"type":"TS", "desc":"Action Date/time"                                        ,"req":true},
   {"type":"CE", "desc":"Problem ID"                                              ,"req":true},
   {"type":"EI", "desc":"Problem Instance ID"                                     ,"req":true},
   {"type":"EI", "desc":"Episode Of Care ID"                                     },
   {"type":"NM", "desc":"Problem List Priority"                                  },
   {"type":"TS", "desc":"Problem Established Date/time"                          },
   {"type":"TS", "desc":"Anticipated Problem Resolution Date/time"               },
   {"type":"TS", "desc":"Actual Problem Resolution Date/time"                    },
   {"type":"CE", "desc":"Problem Classification"                                 },
   {"type":"CE", "desc":"Problem Management Discipline"                           ,"repeats":true},
   {"type":"CE", "desc":"Problem Persistence"                                    },
   {"type":"CE", "desc":"Problem Confirmation Status"                            },
   {"type":"CE", "desc":"Problem Life Cycle Status"                              },
   {"type":"TS", "desc":"Problem Life Cycle Status Date/time"                    },
   {"type":"TS", "desc":"Problem Date Of Onset"                                  },
   {"type":"ST", "desc":"Problem Onset Text"                                     },
   {"type":"CE", "desc":"Problem Ranking"                                        },
   {"type":"CE", "desc":"Certainty Of Problem"                                   },
   {"type":"NM", "desc":"Probability Of Problem (0-1)"                           },
   {"type":"CE", "desc":"Individual Awareness Of Problem"                        },
   {"type":"CE", "desc":"Problem Prognosis"                                      },
   {"type":"CE", "desc":"Individual Awareness Of Prognosis"                      },
   {"type":"ST", "desc":"Family/significant Other Awareness Of Problem/prognosis"},
   {"type":"CE", "desc":"Security/sensitivity"                                   }]},
 "PRC":{
  "desc":"Pricing",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Primary Key Value - Prc" ,"req":true},
   {"type":"CE", "desc":"Facility ID - Prc"       ,"repeats":true},
   {"type":"CE", "desc":"Department"              ,"repeats":true},
   {"type":"IS", "desc":"Valid Patient Classes"   ,"repeats":true},
   {"type":"CP", "desc":"Price"                   ,"repeats":true},
   {"type":"ST", "desc":"Formula"                 ,"repeats":true},
   {"type":"NM", "desc":"Minimum Quantity"       },
   {"type":"NM", "desc":"Maximum Quantity"       },
   {"type":"MO", "desc":"Minimum Price"          },
   {"type":"MO", "desc":"Maximum Price"          },
   {"type":"TS", "desc":"Effective Start Date"   },
   {"type":"TS", "desc":"Effective End Date"     },
   {"type":"IS", "desc":"Price Override Flag"    },
   {"type":"CE", "desc":"Billing Category"        ,"repeats":true},
   {"type":"ID", "desc":"Chargeable Flag"        },
   {"type":"ID", "desc":"Active/inactive Flag"   },
   {"type":"MO", "desc":"Cost"                   },
   {"type":"IS", "desc":"Charge On Indicator"    }]},
 "PRD":{
  "desc":"Provider Data",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Provider Role"                         ,"req":true ,"repeats":true},
   {"type":"XPN", "desc":"Provider Name"                         ,"repeats":true},
   {"type":"XAD", "desc":"Provider Address"                      ,"repeats":true},
   {"type":"PL",  "desc":"Provider Location"                    },
   {"type":"XTN", "desc":"Provider Communication Information"    ,"repeats":true},
   {"type":"CE",  "desc":"Preferred Method Of Contact"          },
   {"type":"PLN", "desc":"Provider Identifiers"                  ,"repeats":true},
   {"type":"TS",  "desc":"Effective Start Date Of Provider Role"},
   {"type":"TS",  "desc":"Effective End Date Of Provider Role"  }]},
 "PRIOR_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_PRIOR",       "desc":""                           },
   {"type":"PATIENT_VISIT_PRIOR", "desc":""                           },
   {"type":"AL1",                 "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"ORDER_PRIOR",         "desc":""                            ,"req":true ,"repeats":true}]},
 "PRIOR_RESULT_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_PRIOR_PRIOR_RESULT",       "desc":""                           },
   {"type":"PATIENT_VISIT_PRIOR_PRIOR_RESULT", "desc":""                           },
   {"type":"AL1",                              "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"ORDER_PRIOR_PRIOR_RESULT",         "desc":""                            ,"req":true ,"repeats":true}]},
 "PRIOR_RESULT_OBSERVATION_REQUEST_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST",       "desc":""                           },
   {"type":"PATIENT_VISIT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST", "desc":""                           },
   {"type":"AL1",                                                  "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST",         "desc":""                            ,"req":true ,"repeats":true}]},
 "PRIOR_RESULT_OBSERVATION_REQUEST_ORDER_SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST_ORDER",       "desc":""                           },
   {"type":"PATIENT_VISIT_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST_ORDER", "desc":""                           },
   {"type":"AL1",                                                        "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST_ORDER",         "desc":""                            ,"req":true ,"repeats":true}]},
 "PRL":{
  "desc":"Parent Result Link",
  "type":"composite",
  "children":[
   {"type":"CE", "desc":"Parent Observation Identifier"      },
   {"type":"ST", "desc":"Parent Observation Sub-identifier"  },
   {"type":"TX", "desc":"Parent Observation Value Descriptor"}]},
 "PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                 "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                 "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                 "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION", "desc":""                   ,"repeats":true}]},
 "PROBLEM_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                         "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                         "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                         "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM", "desc":""                   ,"repeats":true}]},
 "PROBLEM_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                      "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                      "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                      "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY", "desc":""                   ,"repeats":true}]},
 "PROBLEM_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                              "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                              "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                              "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY_PATIENT", "desc":""                   ,"repeats":true}]},
 "PROBLEM_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                             "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                             "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                             "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY_PPGPCH",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY_PPGPCH", "desc":""                   ,"repeats":true}]},
 "PROBLEM_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                             "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                             "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                             "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY_PPGPCJ",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY_PPGPCJ", "desc":""                   ,"repeats":true}]},
 "PROBLEM_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                      "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                      "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                      "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_GOAL_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_GOAL_PATIENT", "desc":""                   ,"repeats":true}]},
 "PROBLEM_GOAL_PGLPC8":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                              "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                              "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                              "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_GOAL",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_GOAL", "desc":""                   ,"repeats":true}]},
 "PROBLEM_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PATHWAY_PPPPCC":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_OBSERVATION_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments" ,"repeats":true}]},
 "PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                 "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                 "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                 "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PATHWAY",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PATHWAY", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM",                        "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM",                       "desc":""                   ,"repeats":true}]},
 "PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                         "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                         "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                         "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PATHWAY_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PATHWAY_PATIENT", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM_PATHWAY_PATIENT",                "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM_PATHWAY_PATIENT",               "desc":""                   ,"repeats":true}]},
 "PROBLEM_PATHWAY_PPPPCC":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                        "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                        "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                        "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PATHWAY_PPPPCC",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PATHWAY_PPPPCC", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM_PATHWAY",                       "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM_PATHWAY",                      "desc":""                   ,"repeats":true}]},
 "PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                        "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                        "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                        "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PATHWAY_PPPPCD",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PATHWAY_PPPPCD", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM_PATHWAY_PPPPCD",                "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM_PATHWAY_PPPPCD",               "desc":""                   ,"repeats":true}]},
 "PROBLEM_PATHWAY_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PTH", "desc":"Pathway"  ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                 "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                 "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                 "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PATIENT",        "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_PATHWAY_PROBLEM",             "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PATIENT", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM_PATIENT",                "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM_PATIENT",               "desc":""                   ,"repeats":true}]},
 "PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PPRPC1",        "desc":""                   ,"repeats":true},
   {"type":"PATHWAY_PROBLEM",                    "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PPRPC1", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM_PPRPC1",                "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM_PPRPC1",               "desc":""                   ,"repeats":true}]},
 "PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PPRPC2",        "desc":""                   ,"repeats":true},
   {"type":"PATHWAY_PROBLEM_PPRPC2",             "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PPRPC2", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM_PPRPC2",                "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM_PPRPC2",               "desc":""                   ,"repeats":true}]},
 "PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRB",                                "desc":"Problem Details"    ,"req":true},
   {"type":"NTE",                                "desc":"Notes And Comments" ,"repeats":true},
   {"type":"VAR",                                "desc":"Variance"           ,"repeats":true},
   {"type":"PROBLEM_ROLE_PROBLEM_PPRPC3",        "desc":""                   ,"repeats":true},
   {"type":"PATHWAY_PROBLEM_PPRPC3",             "desc":""                   ,"repeats":true},
   {"type":"PROBLEM_OBSERVATION_PROBLEM_PPRPC3", "desc":""                   ,"repeats":true},
   {"type":"GOAL_PROBLEM_PPRPC3",                "desc":""                   ,"repeats":true},
   {"type":"ORDER_PROBLEM_PPRPC3",               "desc":""                   ,"repeats":true}]},
 "PROBLEM_ROLE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_GOAL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY_PPGPCH":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_GOAL_PATHWAY_PPGPCJ":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_GOAL_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PATHWAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PATHWAY_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PATHWAY_PPPPCC":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PATHWAY_PPPPCD":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PPRPC1":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PPRPC2":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROBLEM_ROLE_PROBLEM_PPRPC3":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ROL", "desc":"Role"     ,"req":true},
   {"type":"VAR", "desc":"Variance" ,"repeats":true}]},
 "PROCEDURE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA28":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_ADTA31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_BARP10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures"          ,"req":true},
   {"type":"GP2", "desc":"Procedure Line Item"}]},
 "PROCEDURE_BARP12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_RPAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",             "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_2", "desc":""          }]},
 "PROCEDURE_RPAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                       "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_2_PROCEDURE", "desc":""          }]},
 "PROCEDURE_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                              "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_2_PROCEDURE_RPAI10", "desc":""          }]},
 "PROCEDURE_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1",                              "desc":"Procedures" ,"req":true},
   {"type":"AUTHORIZATION_2_PROCEDURE_RPAI11", "desc":""          }]},
 "PROCEDURE_VISIT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PROCEDURE_VISIT_BARP05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PR1", "desc":"Procedures" ,"req":true},
   {"type":"ROL", "desc":"Role"       ,"repeats":true}]},
 "PRODUCT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"BPX", "desc":"Blood Product Dispense Status" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"            ,"repeats":true}]},
 "PRODUCT_FACILITY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PSH", "desc":"Product Summary Header" ,"req":true},
   {"type":"PDC", "desc":"Product Detail Country" ,"req":true}]},
 "PRODUCT_STATUS":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"BTX", "desc":"Blood Product Transfusion/Disposition" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"                    ,"repeats":true}]},
 "PROVIDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT_REFI13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT_REFI14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT_REFI15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT_RRII12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT_RRII13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT_RRII14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_CONTACT_RRII15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RCLI06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPII01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPII04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPLI02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RPRI03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQCI05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQCI06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQII01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQII02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQII03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQII07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PROVIDER_RQPI04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PRD", "desc":"Provider Data" ,"req":true},
   {"type":"CTD", "desc":"Contact Data"  ,"repeats":true}]},
 "PRRPC5":{
  "desc":"Patient problem response",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"QAK",            "desc":"Query Acknowledgment"  },
   {"type":"QRD",            "desc":"Style Query Definition" ,"req":true},
   {"type":"PATIENT_PRRPC5", "desc":""                       ,"req":true ,"repeats":true}]},
 "PSH":{
  "desc":"Product Summary Header",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Report Type"                                               ,"req":true},
   {"type":"ST", "desc":"Report Form Identifier"                                   },
   {"type":"TS", "desc":"Report Date"                                               ,"req":true},
   {"type":"TS", "desc":"Report Interval Start Date"                               },
   {"type":"TS", "desc":"Report Interval End Date"                                 },
   {"type":"CQ", "desc":"Quantity Manufactured"                                    },
   {"type":"CQ", "desc":"Quantity Distributed"                                     },
   {"type":"ID", "desc":"Quantity Distributed Method"                              },
   {"type":"FT", "desc":"Quantity Distributed Comment"                             },
   {"type":"CQ", "desc":"Quantity In Use"                                          },
   {"type":"ID", "desc":"Quantity In Use Method"                                   },
   {"type":"FT", "desc":"Quantity In Use Comment"                                  },
   {"type":"NM", "desc":"Number Of Product Experience Reports Filed By Facility"    ,"repeats":true},
   {"type":"NM", "desc":"Number Of Product Experience Reports Filed By Distributor" ,"repeats":true}]},
 "PT":{
  "desc":"Processing Type",
  "type":"composite",
  "children":[
   {"type":"ID", "desc":"Processing ID"  },
   {"type":"ID", "desc":"Processing Mode"}]},
 "PTA":{
  "desc":"Policy Type And Amount",
  "type":"composite",
  "children":[
   {"type":"IS",  "desc":"Policy Type"                 },
   {"type":"IS",  "desc":"Amount Class"                },
   {"type":"NM",  "desc":"Money Or Percentage Quantity"},
   {"type":"MOP", "desc":"Money Or Percentage"         }]},
 "PTH":{
  "desc":"Pathway",
  "type":"segment",
  "children":[
   {"type":"ID", "desc":"Action Code"                                ,"req":true},
   {"type":"CE", "desc":"Pathway ID"                                 ,"req":true},
   {"type":"EI", "desc":"Pathway Instance ID"                        ,"req":true},
   {"type":"TS", "desc":"Pathway Established Date/time"              ,"req":true},
   {"type":"CE", "desc":"Pathway Life Cycle Status"                 },
   {"type":"TS", "desc":"Change Pathway Life Cycle Status Date/time"}]},
 "PTRPCF":{
  "desc":"Patient pathway (problem-oriented) query response",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"         ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA",            "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",            "desc":"Error"                  ,"repeats":true},
   {"type":"QAK",            "desc":"Query Acknowledgment"  },
   {"type":"QRD",            "desc":"Style Query Definition" ,"req":true},
   {"type":"PATIENT_PTRPCF", "desc":""                       ,"req":true ,"repeats":true}]},
 "PV1":{
  "desc":"Patient Visit",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Pv1"             },
   {"type":"IS",  "desc":"Patient Class"             ,"req":true},
   {"type":"PL",  "desc":"Assigned Patient Location"},
   {"type":"IS",  "desc":"Admission Type"           },
   {"type":"CX",  "desc":"Preadmit Number"          },
   {"type":"PL",  "desc":"Prior Patient Location"   },
   {"type":"XCN", "desc":"Attending Doctor"          ,"repeats":true},
   {"type":"XCN", "desc":"Referring Doctor"          ,"repeats":true},
   {"type":"XCN", "desc":"Consulting Doctor"         ,"repeats":true},
   {"type":"IS",  "desc":"Hospital Service"         },
   {"type":"PL",  "desc":"Temporary Location"       },
   {"type":"IS",  "desc":"Preadmit Test Indicator"  },
   {"type":"IS",  "desc":"Re-admission Indicator"   },
   {"type":"IS",  "desc":"Admit Source"             },
   {"type":"IS",  "desc":"Ambulatory Status"         ,"repeats":true},
   {"type":"IS",  "desc":"Vip Indicator"            },
   {"type":"XCN", "desc":"Admitting Doctor"          ,"repeats":true},
   {"type":"IS",  "desc":"Patient Type"             },
   {"type":"CX",  "desc":"Visit Number"             },
   {"type":"FC",  "desc":"Financial Class"           ,"repeats":true},
   {"type":"IS",  "desc":"Charge Price Indicator"   },
   {"type":"IS",  "desc":"Courtesy Code"            },
   {"type":"IS",  "desc":"Credit Rating"            },
   {"type":"IS",  "desc":"Contract Code"             ,"repeats":true},
   {"type":"DT",  "desc":"Contract Effective Date"   ,"repeats":true},
   {"type":"NM",  "desc":"Contract Amount"           ,"repeats":true},
   {"type":"NM",  "desc":"Contract Period"           ,"repeats":true},
   {"type":"IS",  "desc":"Interest Code"            },
   {"type":"IS",  "desc":"Transfer To Bad Debt Code"},
   {"type":"DT",  "desc":"Transfer To Bad Debt Date"},
   {"type":"IS",  "desc":"Bad Debt Agency Code"     },
   {"type":"NM",  "desc":"Bad Debt Transfer Amount" },
   {"type":"NM",  "desc":"Bad Debt Recovery Amount" },
   {"type":"IS",  "desc":"Delete Account Indicator" },
   {"type":"DT",  "desc":"Delete Account Date"      },
   {"type":"IS",  "desc":"Discharge Disposition"    },
   {"type":"DLD", "desc":"Discharged To Location"   },
   {"type":"CE",  "desc":"Diet Type"                },
   {"type":"IS",  "desc":"Servicing Facility"       },
   {"type":"IS",  "desc":"Bed Status"               },
   {"type":"IS",  "desc":"Account Status"           },
   {"type":"PL",  "desc":"Pending Location"         },
   {"type":"PL",  "desc":"Prior Temporary Location" },
   {"type":"TS",  "desc":"Admit Date/time"          },
   {"type":"TS",  "desc":"Discharge Date/time"       ,"repeats":true},
   {"type":"NM",  "desc":"Current Patient Balance"  },
   {"type":"NM",  "desc":"Total Charges"            },
   {"type":"NM",  "desc":"Total Adjustments"        },
   {"type":"NM",  "desc":"Total Payments"           },
   {"type":"CX",  "desc":"Alternate Visit ID"       },
   {"type":"IS",  "desc":"Visit Indicator"          },
   {"type":"XCN", "desc":"Other Healthcare Provider" ,"repeats":true}]},
 "PV2":{
  "desc":"Additional Information",
  "type":"segment",
  "children":[
   {"type":"PL",  "desc":"Prior Pending Location"                  },
   {"type":"CE",  "desc":"Accommodation Code"                      },
   {"type":"CE",  "desc":"Admit Reason"                            },
   {"type":"CE",  "desc":"Transfer Reason"                         },
   {"type":"ST",  "desc":"Patient Valuables"                        ,"repeats":true},
   {"type":"ST",  "desc":"Patient Valuables Location"              },
   {"type":"IS",  "desc":"Visit User Code"                          ,"repeats":true},
   {"type":"TS",  "desc":"Expected Admit Date/time"                },
   {"type":"TS",  "desc":"Expected Discharge Date/time"            },
   {"type":"NM",  "desc":"Estimated Length Of Inpatient Stay"      },
   {"type":"NM",  "desc":"Actual Length Of Inpatient Stay"         },
   {"type":"ST",  "desc":"Visit Description"                       },
   {"type":"XCN", "desc":"Referral Source Code"                     ,"repeats":true},
   {"type":"DT",  "desc":"Previous Service Date"                   },
   {"type":"ID",  "desc":"Employment Illness Related Indicator"    },
   {"type":"IS",  "desc":"Purge Status Code"                       },
   {"type":"DT",  "desc":"Purge Status Date"                       },
   {"type":"IS",  "desc":"Special Program Code"                    },
   {"type":"ID",  "desc":"Retention Indicator"                     },
   {"type":"NM",  "desc":"Expected Number Of Insurance Plans"      },
   {"type":"IS",  "desc":"Visit Publicity Code"                    },
   {"type":"ID",  "desc":"Visit Protection Indicator"              },
   {"type":"XON", "desc":"Clinic Organization Name"                 ,"repeats":true},
   {"type":"IS",  "desc":"Patient Status Code"                     },
   {"type":"IS",  "desc":"Visit Priority Code"                     },
   {"type":"DT",  "desc":"Previous Treatment Date"                 },
   {"type":"IS",  "desc":"Expected Discharge Disposition"          },
   {"type":"DT",  "desc":"Signature On File Date"                  },
   {"type":"DT",  "desc":"First Similar Illness Date"              },
   {"type":"CE",  "desc":"Patient Charge Adjustment Code"          },
   {"type":"IS",  "desc":"Recurring Service Code"                  },
   {"type":"ID",  "desc":"Billing Media Code"                      },
   {"type":"TS",  "desc":"Expected Surgery Date And Time"          },
   {"type":"ID",  "desc":"Military Partnership Code"               },
   {"type":"ID",  "desc":"Military Non-availability Code"          },
   {"type":"ID",  "desc":"Newborn Baby Indicator"                  },
   {"type":"ID",  "desc":"Baby Detained Indicator"                 },
   {"type":"CE",  "desc":"Mode Of Arrival Code"                    },
   {"type":"CE",  "desc":"Recreational Drug Use Code"               ,"repeats":true},
   {"type":"CE",  "desc":"Admission Level Of Care Code"            },
   {"type":"CE",  "desc":"Precaution Code"                          ,"repeats":true},
   {"type":"CE",  "desc":"Patient Condition Code"                  },
   {"type":"IS",  "desc":"Living Will Code"                        },
   {"type":"IS",  "desc":"Organ Donor Code"                        },
   {"type":"CE",  "desc":"Advance Directive Code"                   ,"repeats":true},
   {"type":"DT",  "desc":"Patient Status Effective Date"           },
   {"type":"TS",  "desc":"Expected Loa Return Date/time"           },
   {"type":"TS",  "desc":"Expected Pre-admission Testing Date/time"},
   {"type":"IS",  "desc":"Notify Clergy Code"                       ,"repeats":true}]},
 "QAK":{
  "desc":"Query Acknowledgment",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Query Tag"            },
   {"type":"ID", "desc":"Query Response Status"},
   {"type":"CE", "desc":"Message Query Name"   },
   {"type":"NM", "desc":"Hit Count"            },
   {"type":"NM", "desc":"This Payload"         },
   {"type":"NM", "desc":"Hits Remaining"       }]},
 "QBPQ11":{
  "desc":"Query by parameter requesting an RSP - segment pattern response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ13":{
  "desc":"Query by parameter requesting an RTB - tabular response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RDF", "desc":"Table Row Definition"      },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ15":{
  "desc":"Query by parameter requesting an RDY - display response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"           },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ21":{
  "desc":"Get person demographics",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ22":{
  "desc":"Find candidates",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ23":{
  "desc":"Get corresponding identifiers",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ24":{
  "desc":"Allocate identifiers",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ25":{
  "desc":"Personnel Information by Segment Query",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPQ31":{
  "desc":"Dispense History",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ73":{
  "desc":"Information about Phone Calls",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true}]},
 "QBPZ75":{
  "desc":"Tabular Patient List",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RDF", "desc":"Table Row Definition"      },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ77":{
  "desc":"Tabular Patient List",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RDF", "desc":"Table Row Definition"      },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ79":{
  "desc":"Dispense Information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"           },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ81":{
  "desc":"Dispense History",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ85":{
  "desc":"Pharmacy Information Comprehensive",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ87":{
  "desc":"Dispense History",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ89":{
  "desc":"Lab Results History",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ91":{
  "desc":"Who Am I",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RDF", "desc":"Table Row Definition"      },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ93":{
  "desc":"Tabular Dispense History",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RDF", "desc":"Table Row Definition"      },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ95":{
  "desc":"Tabular Dispense History",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RDF", "desc":"Table Row Definition"      },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ97":{
  "desc":"Dispense History",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"           },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QBPZ99":{
  "desc":"Who Am I",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RDF", "desc":"Table Row Definition"      },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QCNJ01":{
  "desc":"Cancel query/acknowledge message",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"       ,"req":true},
   {"type":"SFT", "desc":"Software Segment"     ,"repeats":true},
   {"type":"QID", "desc":"Query Identification" ,"req":true}]},
 "QCNJ02":{
  "desc":"Cancel subscription/acknowledge message",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"       ,"req":true},
   {"type":"SFT", "desc":"Software Segment"     ,"repeats":true},
   {"type":"QID", "desc":"Query Identification" ,"req":true}]},
 "QID":{
  "desc":"Query Identification",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Query Tag"          ,"req":true},
   {"type":"CE", "desc":"Message Query Name" ,"req":true}]},
 "QIP":{
  "desc":"Query Input Parameter List",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Segment Field Name"},
   {"type":"ST", "desc":"Values"            }]},
 "QPD":{
  "desc":"Query Parameter Definition",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Message Query Name"                     ,"req":true},
   {"type":"ST", "desc":"Query Tag"                             },
   {"type":"ST", "desc":"User Parameters (in Successive Fields)"}]},
 "QRD":{
  "desc":"Style Query Definition",
  "type":"segment",
  "children":[
   {"type":"TS",  "desc":"Query Date/time"             ,"req":true},
   {"type":"ID",  "desc":"Query Format Code"           ,"req":true},
   {"type":"ID",  "desc":"Query Priority"              ,"req":true},
   {"type":"ST",  "desc":"Query ID"                    ,"req":true},
   {"type":"ID",  "desc":"Deferred Response Type"     },
   {"type":"TS",  "desc":"Deferred Response Date/time"},
   {"type":"CQ",  "desc":"Quantity Limited Request"    ,"req":true},
   {"type":"XCN", "desc":"Who Subject Filter"          ,"req":true ,"repeats":true},
   {"type":"CE",  "desc":"What Subject Filter"         ,"req":true ,"repeats":true},
   {"type":"CE",  "desc":"What Department Data Code"   ,"req":true ,"repeats":true},
   {"type":"VR",  "desc":"What Data Code Value Qual."  ,"repeats":true},
   {"type":"ID",  "desc":"Query Results Level"        }]},
 "QRF":{
  "desc":"Original Style Query Filter",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Where Subject Filter"             ,"req":true ,"repeats":true},
   {"type":"TS", "desc":"When Data Start Date/time"       },
   {"type":"TS", "desc":"When Data End Date/time"         },
   {"type":"ST", "desc":"What User Qualifier"              ,"repeats":true},
   {"type":"ST", "desc":"Other Qry Subject Filter"         ,"repeats":true},
   {"type":"ID", "desc":"Which Date/time Qualifier"        ,"repeats":true},
   {"type":"ID", "desc":"Which Date/time Status Qualifier" ,"repeats":true},
   {"type":"ID", "desc":"Date/time Selection Qualifier"    ,"repeats":true},
   {"type":"TQ", "desc":"When Quantity/timing Qualifier"  },
   {"type":"NM", "desc":"Search Confidence Threshold"     }]},
 "QRI":{
  "desc":"Query Response Instance",
  "type":"segment",
  "children":[
   {"type":"NM", "desc":"Candidate Confidence"},
   {"type":"IS", "desc":"Match Reason Code"    ,"repeats":true},
   {"type":"CE", "desc":"Algorithm Descriptor"}]},
 "QRYA19":{
  "desc":"Patient query",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "QRYPC4":{
  "desc":"Patient problem query",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "QRYPC9":{
  "desc":"Patient goal query",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "QRYPCE":{
  "desc":"Patient pathway (problem-oriented) query",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "QRYPCK":{
  "desc":"Patient pathway (goal-oriented) query",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "QRYQ26":{
  "desc":"Pharmacy/treatment order response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"},
   {"type":"DSC", "desc":"Continuation Pointer"       }]},
 "QRYQ27":{
  "desc":"Pharmacy/treatment administration information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"},
   {"type":"DSC", "desc":"Continuation Pointer"       }]},
 "QRYQ28":{
  "desc":"Pharmacy/treatment dispense information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"},
   {"type":"DSC", "desc":"Continuation Pointer"       }]},
 "QRYQ29":{
  "desc":"Pharmacy/treatment encoded order information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"},
   {"type":"DSC", "desc":"Continuation Pointer"       }]},
 "QRYQ30":{
  "desc":"Pharmacy/treatment dose information",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"},
   {"type":"DSC", "desc":"Continuation Pointer"       }]},
 "QRYR02":{
  "desc":"Query for results of observation",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter" ,"req":true}]},
 "QRYT12":{
  "desc":"Document query",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "QRY_WITH_DETAIL":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "QSBQ16":{
  "desc":"Create subscription",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QSBZ83":{
  "desc":"ORU subscription",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "QSC":{
  "desc":"Query Selection Criteria",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Segment Field Name"    },
   {"type":"ID", "desc":"Relational Operator"   },
   {"type":"ST", "desc":"Value"                 },
   {"type":"ID", "desc":"Relational Conjunction"}]},
 "QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"EVN",       "desc":"Event Type"                      },
   {"type":"PID",       "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",       "desc":"Patient Additional Demographic"  },
   {"type":"ROL",       "desc":"Role"                             ,"repeats":true},
   {"type":"NK1",       "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PV1",       "desc":"Patient Visit"                    ,"req":true},
   {"type":"PV2",       "desc":"Additional Information"          },
   {"type":"ROL",       "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",       "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",       "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",       "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",       "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",       "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE", "desc":""                                 ,"repeats":true},
   {"type":"GT1",       "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE", "desc":""                                 ,"repeats":true},
   {"type":"ACC",       "desc":"Accident"                        },
   {"type":"UB1",       "desc":"UB82"                            },
   {"type":"UB2",       "desc":"UB92 Data"                       }]},
 "QUERY_RESPONSE_ORFR04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_QUERY_RESPONSE", "desc":""},
   {"type":"ORDER_QUERY_RESPONSE",   "desc":"" ,"req":true ,"repeats":true}]},
 "QUERY_RESPONSE_RSPK21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"  },
   {"type":"NK1", "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"QRI", "desc":"Query Response Instance"          ,"req":true}]},
 "QUERY_RESPONSE_RSPK22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID", "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1", "desc":"Patient Additional Demographic"  },
   {"type":"NK1", "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"QRI", "desc":"Query Response Instance"          ,"req":true}]},
 "QUERY_RESPONSE_RSPZ82":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_QUERY_RESPONSE_RSPZ82", "desc":""},
   {"type":"COMMON_ORDER_QUERY_RESPONSE",   "desc":"" ,"req":true ,"repeats":true}]},
 "QUERY_RESPONSE_RSPZ86":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_QUERY_RESPONSE_RSPZ86",      "desc":""},
   {"type":"COMMON_ORDER_QUERY_RESPONSE_RSPZ86", "desc":"" ,"req":true ,"repeats":true}]},
 "QUERY_RESPONSE_RSPZ88":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_QUERY_RESPONSE_RSPZ88",      "desc":""},
   {"type":"COMMON_ORDER_QUERY_RESPONSE_RSPZ88", "desc":"" ,"req":true ,"repeats":true}]},
 "QUERY_RESPONSE_RSPZ90":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_QUERY_RESPONSE_RSPZ90",      "desc":""},
   {"type":"COMMON_ORDER_QUERY_RESPONSE_RSPZ90", "desc":"" ,"req":true ,"repeats":true},
   {"type":"SPECIMEN_QUERY_RESPONSE",            "desc":"" ,"repeats":true}]},
 "QVRQ17":{
  "desc":"Query for previous events",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"Hxx", "desc":"Any Hl7 Segment"           },
   {"type":"RCP", "desc":"Response Control Parameter" ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "RARRAR":{
  "desc":"Pharmacy/treatment administration information - response",
  "type":"message",
  "children":[
   {"type":"MSH",        "desc":"Message Header"         ,"req":true},
   {"type":"MSA",        "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",        "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",        "desc":"Software Segment"       ,"repeats":true},
   {"type":"DEFINITION", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"DSC",        "desc":"Continuation Pointer"  }]},
 "RASO17":{
  "desc":"Pharmacy/treatment administration",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_RASO17", "desc":""                  },
   {"type":"ORDER_RASO17",   "desc":""                   ,"req":true ,"repeats":true}]},
 "RCD":{
  "desc":"Row Column Definition",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Segment Field Name"  },
   {"type":"ID", "desc":"Hl7 Data Type"       },
   {"type":"NM", "desc":"Maximum Column Width"}]},
 "RCII05":{
  "desc":"Request for patient clinical information - response",
  "type":"message",
  "children":[
   {"type":"MSH",      "desc":"Message Header"              ,"req":true},
   {"type":"SFT",      "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",      "desc":"Message Acknowledgment"      ,"req":true},
   {"type":"QRD",      "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",      "desc":"Original Style Query Filter"},
   {"type":"PROVIDER", "desc":""                            ,"req":true ,"repeats":true},
   {"type":"PID",      "desc":"Patient Identification"      ,"req":true},
   {"type":"DG1",      "desc":"Diagnosis"                   ,"repeats":true},
   {"type":"DRG",      "desc":"Diagnosis Related Group"     ,"repeats":true},
   {"type":"AL1",      "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"Group2",   "desc":""                            ,"repeats":true},
   {"type":"NTE",      "desc":"Notes And Comments"          ,"repeats":true}]},
 "RCLI06":{
  "desc":"Request/receipt of clinical data listing - response ",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"              ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",             "desc":"Message Acknowledgment"      ,"req":true},
   {"type":"QRD",             "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",             "desc":"Original Style Query Filter"},
   {"type":"PROVIDER_RCLI06", "desc":""                            ,"req":true ,"repeats":true},
   {"type":"PID",             "desc":"Patient Identification"      ,"req":true},
   {"type":"DG1",             "desc":"Diagnosis"                   ,"repeats":true},
   {"type":"DRG",             "desc":"Diagnosis Related Group"     ,"repeats":true},
   {"type":"AL1",             "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"          ,"repeats":true},
   {"type":"DSP",             "desc":"Display Data"                ,"repeats":true},
   {"type":"DSC",             "desc":"Continuation Pointer"       }]},
 "RCP":{
  "desc":"Response Control Parameter",
  "type":"segment",
  "children":[
   {"type":"ID",  "desc":"Query Priority"             },
   {"type":"CQ",  "desc":"Quantity Limited Request"   },
   {"type":"CE",  "desc":"Response Modality"          },
   {"type":"TS",  "desc":"Execution And Delivery Time"},
   {"type":"ID",  "desc":"Modify Indicator"           },
   {"type":"SRT", "desc":"Sort-by Field"               ,"repeats":true},
   {"type":"ID",  "desc":"Segment Group Inclusion"     ,"repeats":true}]},
 "RDEO11":{
  "desc":"Pharmacy/treatment encoded order",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_RDEO11", "desc":""                  },
   {"type":"ORDER_RDEO11",   "desc":""                   ,"req":true ,"repeats":true}]},
 "RDEO25":{
  "desc":"Pharmacy/treatment refill authorization request",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_RDEO25", "desc":""                  },
   {"type":"ORDER_RDEO25",   "desc":""                   ,"req":true ,"repeats":true}]},
 "RDF":{
  "desc":"Table Row Definition",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Number Of Columns Per Row" ,"req":true},
   {"type":"RCD", "desc":"Column Description"        ,"req":true ,"repeats":true}]},
 "RDRRDR":{
  "desc":"Pharmacy/treatment dispense information - response",
  "type":"message",
  "children":[
   {"type":"MSH",               "desc":"Message Header"         ,"req":true},
   {"type":"MSA",               "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",               "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",               "desc":"Software Segment"       ,"repeats":true},
   {"type":"DEFINITION_RDRRDR", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"DSC",               "desc":"Continuation Pointer"  }]},
 "RDSO13":{
  "desc":"Pharmacy/treatment dispense",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_RDSO13", "desc":""                  },
   {"type":"ORDER_RDSO13",   "desc":""                   ,"req":true ,"repeats":true}]},
 "RDT":{
  "desc":"Table Row Data",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Column Value" ,"req":true}]},
 "RDYK15":{
  "desc":"Display response in response to QBP^Q15",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                     },
   {"type":"QAK", "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"DSP", "desc":"Display Data"               ,"repeats":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "RDYZ98":{
  "desc":"Dispense history - response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                     },
   {"type":"QAK", "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"DSP", "desc":"Display Data"               ,"repeats":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "REFI12":{
  "desc":"Patient referral",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                   "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_CONTACT", "desc":""                                },
   {"type":"PROVIDER_CONTACT",      "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                   "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                   "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",                   "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_REFI12",         "desc":""                                 ,"repeats":true},
   {"type":"ACC",                   "desc":"Accident"                        },
   {"type":"DG1",                   "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                   "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                   "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group4",                "desc":""                                 ,"repeats":true},
   {"type":"Group6_REFI12",         "desc":""                                 ,"repeats":true},
   {"type":"PATIENT_VISIT_REFI12",  "desc":""                                },
   {"type":"NTE",                   "desc":"Notes And Comments"               ,"repeats":true}]},
 "REFI13":{
  "desc":"Modify patient referral",
  "type":"message",
  "children":[
   {"type":"MSH",                          "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                          "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                          "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_CONTACT_REFI13", "desc":""                                },
   {"type":"PROVIDER_CONTACT_REFI13",      "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_REFI13",                "desc":""                                 ,"repeats":true},
   {"type":"ACC",                          "desc":"Accident"                        },
   {"type":"DG1",                          "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                          "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group4_REFI13",                "desc":""                                 ,"repeats":true},
   {"type":"Group6_REFI13",                "desc":""                                 ,"repeats":true},
   {"type":"PATIENT_VISIT_REFI13",         "desc":""                                },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true}]},
 "REFI14":{
  "desc":"Cancel patient referral",
  "type":"message",
  "children":[
   {"type":"MSH",                          "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                          "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                          "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_CONTACT_REFI14", "desc":""                                },
   {"type":"PROVIDER_CONTACT_REFI14",      "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_REFI14",                "desc":""                                 ,"repeats":true},
   {"type":"ACC",                          "desc":"Accident"                        },
   {"type":"DG1",                          "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                          "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group4_REFI14",                "desc":""                                 ,"repeats":true},
   {"type":"Group6_REFI14",                "desc":""                                 ,"repeats":true},
   {"type":"PATIENT_VISIT_REFI14",         "desc":""                                },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true}]},
 "REFI15":{
  "desc":"Request patient referral status",
  "type":"message",
  "children":[
   {"type":"MSH",                          "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                          "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                          "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_CONTACT_REFI15", "desc":""                                },
   {"type":"PROVIDER_CONTACT_REFI15",      "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                          "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                          "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",                          "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_REFI15",                "desc":""                                 ,"repeats":true},
   {"type":"ACC",                          "desc":"Accident"                        },
   {"type":"DG1",                          "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                          "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group4_REFI15",                "desc":""                                 ,"repeats":true},
   {"type":"Group6_REFI15",                "desc":""                                 ,"repeats":true},
   {"type":"PATIENT_VISIT_REFI15",         "desc":""                                },
   {"type":"NTE",                          "desc":"Notes And Comments"               ,"repeats":true}]},
 "REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ARQ",               "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",               "desc":"Appointment Preferences"},
   {"type":"PID",               "desc":"Patient Identification" },
   {"type":"RESOURCES_REQUEST", "desc":""                        ,"req":true ,"repeats":true}]},
 "RERRER":{
  "desc":"Pharmacy/treatment encoded order information - response",
  "type":"message",
  "children":[
   {"type":"MSH",               "desc":"Message Header"         ,"req":true},
   {"type":"MSA",               "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",               "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",               "desc":"Software Segment"       ,"repeats":true},
   {"type":"DEFINITION_RERRER", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"DSC",               "desc":"Continuation Pointer"  }]},
 "RESOURCES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE", "desc":""               ,"repeats":true}]},
 "RESOURCES_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                  "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_REQUEST",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_REQUEST",   "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_REQUEST", "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_REQUEST",  "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                   "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE",   "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE", "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE",  "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS01",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS01",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS01",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS01", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS02",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS02",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS02",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS02", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS03",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS03",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS03",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS03", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS04",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS04",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS04",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS04", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS05",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS05",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS05",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS05", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS06",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS06",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS06",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS06", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS07",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS07",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS07",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS07", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS08",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS08",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS08",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS08", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS09",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS09",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS09",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS09", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS10",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS10",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS10",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS10", "desc":""               ,"repeats":true}]},
 "RESOURCES_SCHEDULE_SRRS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SCHEDULE_SRRS11",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SCHEDULE_SRRS11",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SCHEDULE_SRRS11",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SCHEDULE_SRRS11", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                          "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS14",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS14",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS14",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS14", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS15",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS15",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS15",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS15", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS16",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS16",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS16",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS16", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS17",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS17",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS17",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS17", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS18",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS18",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS18",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS18", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS19",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS19",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS19",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS19", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS20",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS20",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS20",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS20", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS21",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS21",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS21",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS21", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS22",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS22",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS22",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS22", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS23",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS23",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS23",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS23", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS24",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS24",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS24",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS24", "desc":""               ,"repeats":true}]},
 "RESOURCES_SIUS26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SIUS26",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SIUS26",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SIUS26",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SIUS26", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS01",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS01",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS01",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS01", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS02",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS02",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS02",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS02", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS03",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS03",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS03",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS03", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS04",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS04",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS04",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS04", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS05",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS05",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS05",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS05", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS06",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS06",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS06",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS06", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS07",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS07",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS07",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS07", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS08",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS08",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS08",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS08", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS09",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS09",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS09",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS09", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS10",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS10",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS10",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS10", "desc":""               ,"repeats":true}]},
 "RESOURCES_SRMS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RGS",                                 "desc":"Resource Group" ,"req":true},
   {"type":"SERVICE_RESOURCES_SRMS11",            "desc":""               ,"repeats":true},
   {"type":"GENERAL_RESOURCE_RESOURCES_SRMS11",   "desc":""               ,"repeats":true},
   {"type":"LOCATION_RESOURCE_RESOURCES_SRMS11",  "desc":""               ,"repeats":true},
   {"type":"PERSONNEL_RESOURCE_RESOURCES_SRMS11", "desc":""               ,"repeats":true}]},
 "RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PID",            "desc":"Patient Identification"},
   {"type":"ORDER_RESPONSE", "desc":""                       ,"repeats":true}]},
 "RESPONSE_ORDO04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE",    "desc":""},
   {"type":"ORDER_DIET_RESPONSE", "desc":"" ,"req":true ,"repeats":true},
   {"type":"ORDER_TRAY_RESPONSE", "desc":"" ,"repeats":true}]},
 "RESPONSE_ORGO20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_ORGO20", "desc":""},
   {"type":"ORDER_RESPONSE_ORGO20",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_ORIO24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_ORIO24", "desc":""},
   {"type":"ORDER_RESPONSE_ORIO24",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_ORNO08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_ORNO08", "desc":""},
   {"type":"ORDER_RESPONSE_ORNO08",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_ORPO10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_ORPO10", "desc":""},
   {"type":"ORDER_RESPONSE_ORPO10",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_ORRO02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_ORRO02", "desc":""},
   {"type":"ORDER_RESPONSE_ORRO02",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_ORSO06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_ORSO06", "desc":""},
   {"type":"ORDER_RESPONSE_ORSO06",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_OSRQ06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_OSRQ06", "desc":""},
   {"type":"ORDER_RESPONSE_OSRQ06",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_RRAO18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_RRAO18", "desc":""},
   {"type":"ORDER_RESPONSE_RRAO18",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_RRDO14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_RRDO14", "desc":""},
   {"type":"ORDER_RESPONSE_RRDO14",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_RREO12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_RREO12", "desc":""},
   {"type":"ORDER_RESPONSE_RREO12",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_RREO26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_RREO26", "desc":""},
   {"type":"ORDER_RESPONSE_RREO26",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_RRGO16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_RRGO16", "desc":""},
   {"type":"ORDER_RESPONSE_RRGO16",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESPONSE_RSPK31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PATIENT_RESPONSE_RSPK31", "desc":""},
   {"type":"ORDER_RESPONSE_RSPK31",   "desc":"" ,"req":true ,"repeats":true}]},
 "RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"EVN", "desc":"Event Type"                   },
   {"type":"PID", "desc":"Patient Identification"        ,"req":true},
   {"type":"PV1", "desc":"Patient Visit"                 ,"req":true},
   {"type":"TXA", "desc":"Transcription Document Header" ,"req":true},
   {"type":"OBX", "desc":"Observation/Result"            ,"repeats":true}]},
 "RESULT_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"   ,"req":true},
   {"type":"TCD", "desc":"Test Code Detail"    },
   {"type":"SID", "desc":"Substance Identifier" ,"repeats":true},
   {"type":"NTE", "desc":"Notes And Comments"   ,"repeats":true}]},
 "RESULT_ORDER_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"   ,"req":true},
   {"type":"TCD", "desc":"Test Code Detail"    },
   {"type":"SID", "desc":"Substance Identifier" ,"repeats":true},
   {"type":"NTE", "desc":"Notes And Comments"   ,"repeats":true}]},
 "RESULT_ORDER_OULR24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"OBX", "desc":"Observation/Result"   ,"req":true},
   {"type":"TCD", "desc":"Test Code Detail"    },
   {"type":"SID", "desc":"Substance Identifier" ,"repeats":true},
   {"type":"NTE", "desc":"Notes And Comments"   ,"repeats":true}]},
 "RF1":{
  "desc":"Referral Information",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Referral Status"                },
   {"type":"CE", "desc":"Referral Priority"              },
   {"type":"CE", "desc":"Referral Type"                  },
   {"type":"CE", "desc":"Referral Disposition"            ,"repeats":true},
   {"type":"CE", "desc":"Referral Category"              },
   {"type":"EI", "desc":"Originating Referral Identifier" ,"req":true},
   {"type":"TS", "desc":"Effective Date"                 },
   {"type":"TS", "desc":"Expiration Date"                },
   {"type":"TS", "desc":"Process Date"                   },
   {"type":"CE", "desc":"Referral Reason"                 ,"repeats":true},
   {"type":"EI", "desc":"External Referral Identifier"    ,"repeats":true}]},
 "RFR":{
  "desc":"Reference Range",
  "type":"composite",
  "children":[
   {"type":"NR", "desc":"Numeric Range"        },
   {"type":"IS", "desc":"Administrative Sex"   },
   {"type":"NR", "desc":"Age Range"            },
   {"type":"NR", "desc":"Gestational Age Range"},
   {"type":"ST", "desc":"Species"              },
   {"type":"ST", "desc":"Race/subspecies"      },
   {"type":"TX", "desc":"Conditions"           }]},
 "RGRRGR":{
  "desc":"Pharmacy/treatment dose information - response",
  "type":"message",
  "children":[
   {"type":"MSH",               "desc":"Message Header"         ,"req":true},
   {"type":"MSA",               "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",               "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",               "desc":"Software Segment"       ,"repeats":true},
   {"type":"DEFINITION_RGRRGR", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"DSC",               "desc":"Continuation Pointer"  }]},
 "RGS":{
  "desc":"Resource Group",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Rgs"        ,"req":true},
   {"type":"ID", "desc":"Segment Action Code"},
   {"type":"CE", "desc":"Resource Group ID"  }]},
 "RGVO15":{
  "desc":"Pharmacy/treatment give",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"     ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"   ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments" ,"repeats":true},
   {"type":"PATIENT_RGVO15", "desc":""                  },
   {"type":"ORDER_RGVO15",   "desc":""                   ,"req":true ,"repeats":true}]},
 "RI":{
  "desc":"Repeat Interval",
  "type":"composite",
  "children":[
   {"type":"IS", "desc":"Repeat Pattern"        },
   {"type":"ST", "desc":"Explicit Time Interval"}]},
 "RMC":{
  "desc":"Room Coverage",
  "type":"composite",
  "children":[
   {"type":"IS",  "desc":"Room Type"          },
   {"type":"IS",  "desc":"Amount Type"        },
   {"type":"NM",  "desc":"Coverage Amount"    },
   {"type":"MOP", "desc":"Money Or Percentage"}]},
 "RMI":{
  "desc":"Risk Management Incident",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Risk Management Incident Code"},
   {"type":"TS", "desc":"Date/time Incident"           },
   {"type":"CE", "desc":"Incident Type Code"           }]},
 "ROL":{
  "desc":"Role",
  "type":"segment",
  "children":[
   {"type":"EI",  "desc":"Role Instance ID"              },
   {"type":"ID",  "desc":"Action Code"                    ,"req":true},
   {"type":"CE",  "desc":"Role-rol"                       ,"req":true},
   {"type":"XCN", "desc":"Role Person"                    ,"req":true ,"repeats":true},
   {"type":"TS",  "desc":"Role Begin Date/time"          },
   {"type":"TS",  "desc":"Role End Date/time"            },
   {"type":"CE",  "desc":"Role Duration"                 },
   {"type":"CE",  "desc":"Role Action Reason"            },
   {"type":"CE",  "desc":"Provider Type"                  ,"repeats":true},
   {"type":"CE",  "desc":"Organization Unit Type"        },
   {"type":"XAD", "desc":"Office/home Address/birthplace" ,"repeats":true},
   {"type":"XTN", "desc":"Phone"                          ,"repeats":true}]},
 "RORROR":{
  "desc":"Pharmacy prescription order query - response",
  "type":"message",
  "children":[
   {"type":"MSH",               "desc":"Message Header"         ,"req":true},
   {"type":"MSA",               "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",               "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",               "desc":"Software Segment"       ,"repeats":true},
   {"type":"DEFINITION_RORROR", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"DSC",               "desc":"Continuation Pointer"  }]},
 "ROW_DEFINITION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RSPZ80":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RSPZ84":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RTBK13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RTBZ74":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RTBZ76":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RTBZ78":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RTBZ92":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RTBZ94":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "ROW_DEFINITION_RTBZ96":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RDF", "desc":"Table Row Definition" ,"req":true},
   {"type":"RDT", "desc":"Table Row Data"       ,"repeats":true}]},
 "RPAI08":{
  "desc":"Request for treatment authorization information - response",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"MSA",              "desc":"Message Acknowledgment"           ,"req":true},
   {"type":"RF1",              "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_1",  "desc":""                                },
   {"type":"PROVIDER_RPAI08",  "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_RPAI08",    "desc":""                                 ,"repeats":true},
   {"type":"ACC",              "desc":"Accident"                        },
   {"type":"DG1",              "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",              "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",              "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"PROCEDURE_RPAI08", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"Group6_RPAI08",    "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RPAI08",     "desc":""                                },
   {"type":"NTE",              "desc":"Notes And Comments"               ,"repeats":true}]},
 "RPAI09":{
  "desc":"Request for modification to an authorization - response",
  "type":"message",
  "children":[
   {"type":"MSH",                    "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                    "desc":"Software Segment"                 ,"repeats":true},
   {"type":"MSA",                    "desc":"Message Acknowledgment"           ,"req":true},
   {"type":"RF1",                    "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_1_RPAI09", "desc":""                                },
   {"type":"PROVIDER_RPAI09",        "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                    "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                    "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",                    "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_RPAI09",          "desc":""                                 ,"repeats":true},
   {"type":"ACC",                    "desc":"Accident"                        },
   {"type":"DG1",                    "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                    "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                    "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"PROCEDURE_RPAI09",       "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"Group6_RPAI09",          "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RPAI09",           "desc":""                                },
   {"type":"NTE",                    "desc":"Notes And Comments"               ,"repeats":true}]},
 "RPAI10":{
  "desc":"Request for resubmission of an authorization - response",
  "type":"message",
  "children":[
   {"type":"MSH",                    "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                    "desc":"Software Segment"                 ,"repeats":true},
   {"type":"MSA",                    "desc":"Message Acknowledgment"           ,"req":true},
   {"type":"RF1",                    "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_1_RPAI10", "desc":""                                },
   {"type":"PROVIDER_RPAI10",        "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                    "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                    "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",                    "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_RPAI10",          "desc":""                                 ,"repeats":true},
   {"type":"ACC",                    "desc":"Accident"                        },
   {"type":"DG1",                    "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                    "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                    "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"PROCEDURE_RPAI10",       "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"Group6_RPAI10",          "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RPAI10",           "desc":""                                },
   {"type":"NTE",                    "desc":"Notes And Comments"               ,"repeats":true}]},
 "RPAI11":{
  "desc":"Request for cancellation of an authorization - response",
  "type":"message",
  "children":[
   {"type":"MSH",                    "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                    "desc":"Software Segment"                 ,"repeats":true},
   {"type":"MSA",                    "desc":"Message Acknowledgment"           ,"req":true},
   {"type":"RF1",                    "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_1_RPAI11", "desc":""                                },
   {"type":"PROVIDER_RPAI11",        "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                    "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                    "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",                    "desc":"Guarantor"                        ,"repeats":true},
   {"type":"Group3_RPAI11",          "desc":""                                 ,"repeats":true},
   {"type":"ACC",                    "desc":"Accident"                        },
   {"type":"DG1",                    "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                    "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                    "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"PROCEDURE_RPAI11",       "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"Group6_RPAI11",          "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RPAI11",           "desc":""                                },
   {"type":"NTE",                    "desc":"Notes And Comments"               ,"repeats":true}]},
 "RPII01":{
  "desc":"Request for insurance information - response",
  "type":"message",
  "children":[
   {"type":"MSH",                 "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                 "desc":"Software Segment"                 ,"repeats":true},
   {"type":"MSA",                 "desc":"Message Acknowledgment"           ,"req":true},
   {"type":"PROVIDER_RPII01",     "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                 "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                 "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE", "desc":""                                },
   {"type":"NTE",                 "desc":"Notes And Comments"               ,"repeats":true}]},
 "RPII04":{
  "desc":"Request for patient demographic data - response",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"MSA",                        "desc":"Message Acknowledgment"           ,"req":true},
   {"type":"PROVIDER_RPII04",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RPII04", "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RPLI02":{
  "desc":"Request/receipt of patient selection display list - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"PROVIDER_RPLI02", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"DSP",             "desc":"Display Data"           ,"repeats":true},
   {"type":"DSC",             "desc":"Continuation Pointer"  }]},
 "RPRI03":{
  "desc":"Request/receipt of patient selection list - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"PROVIDER_RPRI03", "desc":""                       ,"req":true ,"repeats":true},
   {"type":"PID",             "desc":"Patient Identification" ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true}]},
 "RPT":{
  "desc":"Repeat Pattern",
  "type":"composite",
  "children":[
   {"type":"CWE", "desc":"Repeat Pattern Code"         },
   {"type":"ID",  "desc":"Calendar Alignment"          },
   {"type":"NM",  "desc":"Phase Range Begin Value"     },
   {"type":"NM",  "desc":"Phase Range End Value"       },
   {"type":"NM",  "desc":"Period Quantity"             },
   {"type":"IS",  "desc":"Period Units"                },
   {"type":"ID",  "desc":"Institution Specified Time"  },
   {"type":"ID",  "desc":"Event"                       },
   {"type":"NM",  "desc":"Event Offset Quantity"       },
   {"type":"IS",  "desc":"Event Offset Units"          },
   {"type":"GTS", "desc":"General Timing Specification"}]},
 "RQ1":{
  "desc":"Requisition Detail - Additional Information",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"Anticipated Price"      },
   {"type":"CE", "desc":"Manufacturer Identifier"},
   {"type":"ST", "desc":"Manufacturer's Catalog" },
   {"type":"CE", "desc":"Vendor ID"              },
   {"type":"ST", "desc":"Vendor Catalog"         },
   {"type":"ID", "desc":"Taxable"                },
   {"type":"ID", "desc":"Substitute Allowed"     }]},
 "RQAI08":{
  "desc":"Request for treatment authorization information",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                        "desc":"Referral Information"            },
   {"type":"AUTHORIZATION",              "desc":""                                },
   {"type":"PROVIDER_RQAI08",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQAI08", "desc":""                                },
   {"type":"ACC",                        "desc":"Accident"                        },
   {"type":"DG1",                        "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                        "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                        "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group5",                     "desc":""                                 ,"repeats":true},
   {"type":"Group7_RQAI08",              "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RQAI08",               "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQAI09":{
  "desc":"Request for modification to an authorization",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                        "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_RQAI09",       "desc":""                                },
   {"type":"PROVIDER_RQAI09",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQAI09", "desc":""                                },
   {"type":"ACC",                        "desc":"Accident"                        },
   {"type":"DG1",                        "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                        "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                        "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group5_RQAI09",              "desc":""                                 ,"repeats":true},
   {"type":"Group7_RQAI09",              "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RQAI09",               "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQAI10":{
  "desc":"Request for resubmission of an authorization",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                        "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_RQAI10",       "desc":""                                },
   {"type":"PROVIDER_RQAI10",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQAI10", "desc":""                                },
   {"type":"ACC",                        "desc":"Accident"                        },
   {"type":"DG1",                        "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                        "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                        "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group5_RQAI10",              "desc":""                                 ,"repeats":true},
   {"type":"Group7_RQAI10",              "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RQAI10",               "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQAI11":{
  "desc":"Request for cancellation of an authorization",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"RF1",                        "desc":"Referral Information"            },
   {"type":"AUTHORIZATION_RQAI11",       "desc":""                                },
   {"type":"PROVIDER_RQAI11",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQAI11", "desc":""                                },
   {"type":"ACC",                        "desc":"Accident"                        },
   {"type":"DG1",                        "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                        "desc":"Diagnosis Related Group"          ,"repeats":true},
   {"type":"AL1",                        "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"Group5_RQAI11",              "desc":""                                 ,"repeats":true},
   {"type":"Group7_RQAI11",              "desc":""                                 ,"repeats":true},
   {"type":"VISIT_RQAI11",               "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQCI05":{
  "desc":"Request for patient clinical information",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"                 ,"repeats":true},
   {"type":"QRD",             "desc":"Style Query Definition"           ,"req":true},
   {"type":"QRF",             "desc":"Original Style Query Filter"     },
   {"type":"PROVIDER_RQCI05", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",             "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",             "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",             "desc":"Guarantor"                        ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQCI06":{
  "desc":"Request/receipt of clinical data listing",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"                 ,"repeats":true},
   {"type":"QRD",             "desc":"Style Query Definition"           ,"req":true},
   {"type":"QRF",             "desc":"Original Style Query Filter"     },
   {"type":"PROVIDER_RQCI06", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",             "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",             "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",             "desc":"Guarantor"                        ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQD":{
  "desc":"Requisition Detail",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Requisition Line Number"    },
   {"type":"CE", "desc":"Item Code - Internal"       },
   {"type":"CE", "desc":"Item Code - External"       },
   {"type":"CE", "desc":"Hospital Item Code"         },
   {"type":"NM", "desc":"Requisition Quantity"       },
   {"type":"CE", "desc":"Requisition Unit Of Measure"},
   {"type":"IS", "desc":"Dept. Cost Center"          },
   {"type":"IS", "desc":"Item Natural Account Code"  },
   {"type":"CE", "desc":"Deliver To ID"              },
   {"type":"DT", "desc":"Date Needed"                }]},
 "RQII01":{
  "desc":"Request for insurance information",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"PROVIDER_RQII01",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQII01", "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQII02":{
  "desc":"Request/receipt of patient selection display list",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"PROVIDER_RQII02",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQII02", "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQII03":{
  "desc":"Request/receipt of patient selection list",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"PROVIDER_RQII03",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQII03", "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQII07":{
  "desc":"Unsolicited insurance information",
  "type":"message",
  "children":[
   {"type":"MSH",                        "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",                        "desc":"Software Segment"                 ,"repeats":true},
   {"type":"PROVIDER_RQII07",            "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",                        "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",                        "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GUARANTOR_INSURANCE_RQII07", "desc":""                                },
   {"type":"NTE",                        "desc":"Notes And Comments"               ,"repeats":true}]},
 "RQPI04":{
  "desc":"Request for patient demographic data",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",             "desc":"Software Segment"                 ,"repeats":true},
   {"type":"PROVIDER_RQPI04", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"PID",             "desc":"Patient Identification"           ,"req":true},
   {"type":"NK1",             "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"GT1",             "desc":"Guarantor"                        ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"               ,"repeats":true}]},
 "RRAO18":{
  "desc":"Pharmacy/treatment administration acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_RRAO18", "desc":""                      }]},
 "RRDO14":{
  "desc":"Pharmacy/treatment dispense acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_RRDO14", "desc":""                      }]},
 "RREO12":{
  "desc":"Pharmacy/treatment encoded order acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_RREO12", "desc":""                      }]},
 "RREO26":{
  "desc":"Pharmacy/Treatment Refill Authorization Acknowledgement",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_RREO26", "desc":""                      }]},
 "RRGO16":{
  "desc":"Pharmacy/treatment give acknowledgment",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"       ,"repeats":true},
   {"type":"NTE",             "desc":"Notes And Comments"     ,"repeats":true},
   {"type":"RESPONSE_RRGO16", "desc":""                      }]},
 "RRII12":{
  "desc":"Patient referral - response",
  "type":"message",
  "children":[
   {"type":"MSH",                          "desc":"Message Header"              ,"req":true},
   {"type":"SFT",                          "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",                          "desc":"Message Acknowledgment"     },
   {"type":"RF1",                          "desc":"Referral Information"       },
   {"type":"AUTHORIZATION_CONTACT_RRII12", "desc":""                           },
   {"type":"PROVIDER_CONTACT_RRII12",      "desc":""                            ,"req":true ,"repeats":true},
   {"type":"PID",                          "desc":"Patient Identification"      ,"req":true},
   {"type":"ACC",                          "desc":"Accident"                   },
   {"type":"DG1",                          "desc":"Diagnosis"                   ,"repeats":true},
   {"type":"DRG",                          "desc":"Diagnosis Related Group"     ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"Group3_RRII12",                "desc":""                            ,"repeats":true},
   {"type":"Group5_RRII12",                "desc":""                            ,"repeats":true},
   {"type":"PATIENT_VISIT_RRII12",         "desc":""                           },
   {"type":"NTE",                          "desc":"Notes And Comments"          ,"repeats":true}]},
 "RRII13":{
  "desc":"Modify patient referral - response",
  "type":"message",
  "children":[
   {"type":"MSH",                          "desc":"Message Header"              ,"req":true},
   {"type":"SFT",                          "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",                          "desc":"Message Acknowledgment"     },
   {"type":"RF1",                          "desc":"Referral Information"       },
   {"type":"AUTHORIZATION_CONTACT_RRII13", "desc":""                           },
   {"type":"PROVIDER_CONTACT_RRII13",      "desc":""                            ,"req":true ,"repeats":true},
   {"type":"PID",                          "desc":"Patient Identification"      ,"req":true},
   {"type":"ACC",                          "desc":"Accident"                   },
   {"type":"DG1",                          "desc":"Diagnosis"                   ,"repeats":true},
   {"type":"DRG",                          "desc":"Diagnosis Related Group"     ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"Group3_RRII13",                "desc":""                            ,"repeats":true},
   {"type":"Group5_RRII13",                "desc":""                            ,"repeats":true},
   {"type":"PATIENT_VISIT_RRII13",         "desc":""                           },
   {"type":"NTE",                          "desc":"Notes And Comments"          ,"repeats":true}]},
 "RRII14":{
  "desc":"Cancel patient referral - response",
  "type":"message",
  "children":[
   {"type":"MSH",                          "desc":"Message Header"              ,"req":true},
   {"type":"SFT",                          "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",                          "desc":"Message Acknowledgment"     },
   {"type":"RF1",                          "desc":"Referral Information"       },
   {"type":"AUTHORIZATION_CONTACT_RRII14", "desc":""                           },
   {"type":"PROVIDER_CONTACT_RRII14",      "desc":""                            ,"req":true ,"repeats":true},
   {"type":"PID",                          "desc":"Patient Identification"      ,"req":true},
   {"type":"ACC",                          "desc":"Accident"                   },
   {"type":"DG1",                          "desc":"Diagnosis"                   ,"repeats":true},
   {"type":"DRG",                          "desc":"Diagnosis Related Group"     ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"Group3_RRII14",                "desc":""                            ,"repeats":true},
   {"type":"Group5_RRII14",                "desc":""                            ,"repeats":true},
   {"type":"PATIENT_VISIT_RRII14",         "desc":""                           },
   {"type":"NTE",                          "desc":"Notes And Comments"          ,"repeats":true}]},
 "RRII15":{
  "desc":"Request patient referral status - response",
  "type":"message",
  "children":[
   {"type":"MSH",                          "desc":"Message Header"              ,"req":true},
   {"type":"SFT",                          "desc":"Software Segment"            ,"repeats":true},
   {"type":"MSA",                          "desc":"Message Acknowledgment"     },
   {"type":"RF1",                          "desc":"Referral Information"       },
   {"type":"AUTHORIZATION_CONTACT_RRII15", "desc":""                           },
   {"type":"PROVIDER_CONTACT_RRII15",      "desc":""                            ,"req":true ,"repeats":true},
   {"type":"PID",                          "desc":"Patient Identification"      ,"req":true},
   {"type":"ACC",                          "desc":"Accident"                   },
   {"type":"DG1",                          "desc":"Diagnosis"                   ,"repeats":true},
   {"type":"DRG",                          "desc":"Diagnosis Related Group"     ,"repeats":true},
   {"type":"AL1",                          "desc":"Patient Allergy Information" ,"repeats":true},
   {"type":"Group3_RRII15",                "desc":""                            ,"repeats":true},
   {"type":"Group5_RRII15",                "desc":""                            ,"repeats":true},
   {"type":"PATIENT_VISIT_RRII15",         "desc":""                           },
   {"type":"NTE",                          "desc":"Notes And Comments"          ,"repeats":true}]},
 "RSPK11":{
  "desc":"Segment pattern response in response to QBP^Q11",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"             ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",            "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",            "desc":"Error"                     },
   {"type":"QAK",            "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",            "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION", "desc":""                          },
   {"type":"DSC",            "desc":"Continuation Pointer"      }]},
 "RSPK21":{
  "desc":"Get person demographics response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"QUERY_RESPONSE_RSPK21", "desc":""                           ,"repeats":true},
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RSPK22":{
  "desc":"Find candidates response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"QUERY_RESPONSE_RSPK22", "desc":""                           ,"repeats":true},
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RSPK23":{
  "desc":"Get corresponding identifiers response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                     },
   {"type":"QAK", "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"PID", "desc":"Patient Identification"     ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "RSPK24":{
  "desc":"Allocate identifiers response",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"             ,"req":true},
   {"type":"SFT", "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA", "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR", "desc":"Error"                     },
   {"type":"QAK", "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD", "desc":"Query Parameter Definition" ,"req":true},
   {"type":"PID", "desc":"Patient Identification"     ,"req":true},
   {"type":"DSC", "desc":"Continuation Pointer"      }]},
 "RSPK25":{
  "desc":"Personnel Information by Segment Response",
  "type":"message",
  "children":[
   {"type":"MSH",   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",   "desc":"Error"                      ,"repeats":true},
   {"type":"QAK",   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP",   "desc":"Response Control Parameter" ,"req":true},
   {"type":"STAFF", "desc":""                           ,"req":true ,"repeats":true},
   {"type":"DSC",   "desc":"Continuation Pointer"      }]},
 "RSPK31":{
  "desc":"None",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"             ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",             "desc":"Error"                      ,"repeats":true},
   {"type":"SFT",             "desc":"Software Segment"           ,"repeats":true},
   {"type":"QAK",             "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",             "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP",             "desc":"Response Control Parameter" ,"req":true},
   {"type":"RESPONSE_RSPK31", "desc":""                           ,"req":true ,"repeats":true},
   {"type":"DSC",             "desc":"Continuation Pointer"      }]},
 "RSPZ80":{
  "desc":"Dispense information - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RSPZ80", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RSPZ82":{
  "desc":"Dispense history - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP",                   "desc":"Response Control Parameter" ,"req":true},
   {"type":"QUERY_RESPONSE_RSPZ82", "desc":""                           ,"req":true ,"repeats":true},
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RSPZ84":{
  "desc":"Who am I - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RSPZ84", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RSPZ86":{
  "desc":"Pharmacy Information Comprehensive - Response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"QUERY_RESPONSE_RSPZ86", "desc":""                           ,"req":true ,"repeats":true},
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RSPZ88":{
  "desc":"Dispense Information - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP",                   "desc":"Response Control Parameter" ,"req":true},
   {"type":"QUERY_RESPONSE_RSPZ88", "desc":""                           ,"req":true ,"repeats":true},
   {"type":"DSC",                   "desc":"Continuation Pointer"       ,"req":true}]},
 "RSPZ90":{
  "desc":"Lab Results History - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"RCP",                   "desc":"Response Control Parameter" ,"req":true},
   {"type":"QUERY_RESPONSE_RSPZ90", "desc":""                           ,"req":true ,"repeats":true},
   {"type":"DSC",                   "desc":"Continuation Pointer"       ,"req":true}]},
 "RTBK13":{
  "desc":"Tabular response in response to QBP^Q13",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RTBK13", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RTBZ74":{
  "desc":"Information about Phone Calls - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                      ,"repeats":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RTBZ74", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RTBZ76":{
  "desc":"Tabular Patient List - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RTBZ76", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RTBZ78":{
  "desc":"Tabular Patient List - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RTBZ78", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RTBZ92":{
  "desc":"Who Am I - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RTBZ92", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RTBZ94":{
  "desc":"Tabular Dispense History - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RTBZ94", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RTBZ96":{
  "desc":"Tabular Dispense History - response",
  "type":"message",
  "children":[
   {"type":"MSH",                   "desc":"Message Header"             ,"req":true},
   {"type":"SFT",                   "desc":"Software Segment"           ,"repeats":true},
   {"type":"MSA",                   "desc":"Message Acknowledgment"     ,"req":true},
   {"type":"ERR",                   "desc":"Error"                     },
   {"type":"QAK",                   "desc":"Query Acknowledgment"       ,"req":true},
   {"type":"QPD",                   "desc":"Query Parameter Definition" ,"req":true},
   {"type":"ROW_DEFINITION_RTBZ96", "desc":""                          },
   {"type":"DSC",                   "desc":"Continuation Pointer"      }]},
 "RXA":{
  "desc":"Pharmacy/Treatment Administration",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Give Sub-id Counter"                     ,"req":true},
   {"type":"NM",  "desc":"Administration Sub-id Counter"           ,"req":true},
   {"type":"TS",  "desc":"Date/time Start Of Administration"       ,"req":true},
   {"type":"TS",  "desc":"Date/time End Of Administration"         ,"req":true},
   {"type":"CE",  "desc":"Administered Code"                       ,"req":true},
   {"type":"NM",  "desc":"Administered Amount"                     ,"req":true},
   {"type":"CE",  "desc":"Administered Units"                     },
   {"type":"CE",  "desc":"Administered Dosage Form"               },
   {"type":"CE",  "desc":"Administration Notes"                    ,"repeats":true},
   {"type":"XCN", "desc":"Administering Provider"                  ,"repeats":true},
   {"type":"LA2", "desc":"Administered-at Location"               },
   {"type":"ST",  "desc":"Administered Per (time Unit)"           },
   {"type":"NM",  "desc":"Administered Strength"                  },
   {"type":"CE",  "desc":"Administered Strength Units"            },
   {"type":"ST",  "desc":"Substance Lot Number"                    ,"repeats":true},
   {"type":"TS",  "desc":"Substance Expiration Date"               ,"repeats":true},
   {"type":"CE",  "desc":"Substance Manufacturer Name"             ,"repeats":true},
   {"type":"CE",  "desc":"Substance/treatment Refusal Reason"      ,"repeats":true},
   {"type":"CE",  "desc":"Indication"                              ,"repeats":true},
   {"type":"ID",  "desc":"Completion Status"                      },
   {"type":"ID",  "desc":"Action Code - Rxa"                      },
   {"type":"TS",  "desc":"System Entry Date/time"                 },
   {"type":"NM",  "desc":"Administered Drug Strength Volume"      },
   {"type":"CWE", "desc":"Administered Drug Strength Volume Units"},
   {"type":"CWE", "desc":"Administered Barcode Identifier"        },
   {"type":"ID",  "desc":"Pharmacy Order Type"                    }]},
 "RXC":{
  "desc":"Pharmacy/Treatment Component Order",
  "type":"segment",
  "children":[
   {"type":"ID",  "desc":"Rx Component Type"                    ,"req":true},
   {"type":"CE",  "desc":"Component Code"                       ,"req":true},
   {"type":"NM",  "desc":"Component Amount"                     ,"req":true},
   {"type":"CE",  "desc":"Component Units"                      ,"req":true},
   {"type":"NM",  "desc":"Component Strength"                  },
   {"type":"CE",  "desc":"Component Strength Units"            },
   {"type":"CE",  "desc":"Supplementary Code"                   ,"repeats":true},
   {"type":"NM",  "desc":"Component Drug Strength Volume"      },
   {"type":"CWE", "desc":"Component Drug Strength Volume Units"}]},
 "RXD":{
  "desc":"Pharmacy/Treatment Dispense",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Dispense Sub-id Counter"                                       ,"req":true},
   {"type":"CE",  "desc":"Dispense/give Code"                                            ,"req":true},
   {"type":"TS",  "desc":"Date/time Dispensed"                                           ,"req":true},
   {"type":"NM",  "desc":"Actual Dispense Amount"                                        ,"req":true},
   {"type":"CE",  "desc":"Actual Dispense Units"                                        },
   {"type":"CE",  "desc":"Actual Dosage Form"                                           },
   {"type":"ST",  "desc":"Prescription Number"                                           ,"req":true},
   {"type":"NM",  "desc":"Number Of Refills Remaining"                                  },
   {"type":"ST",  "desc":"Dispense Notes"                                                ,"repeats":true},
   {"type":"XCN", "desc":"Dispensing Provider"                                           ,"repeats":true},
   {"type":"ID",  "desc":"Substitution Status"                                          },
   {"type":"CQ",  "desc":"Total Daily Dose"                                             },
   {"type":"LA2", "desc":"Dispense-to Location"                                         },
   {"type":"ID",  "desc":"Needs Human Review"                                           },
   {"type":"CE",  "desc":"Pharmacy/treatment Supplier's Special Dispensing Instructions" ,"repeats":true},
   {"type":"NM",  "desc":"Actual Strength"                                              },
   {"type":"CE",  "desc":"Actual Strength Unit"                                         },
   {"type":"ST",  "desc":"Substance Lot Number"                                          ,"repeats":true},
   {"type":"TS",  "desc":"Substance Expiration Date"                                     ,"repeats":true},
   {"type":"CE",  "desc":"Substance Manufacturer Name"                                   ,"repeats":true},
   {"type":"CE",  "desc":"Indication"                                                    ,"repeats":true},
   {"type":"NM",  "desc":"Dispense Package Size"                                        },
   {"type":"CE",  "desc":"Dispense Package Size Unit"                                   },
   {"type":"ID",  "desc":"Dispense Package Method"                                      },
   {"type":"CE",  "desc":"Supplementary Code"                                            ,"repeats":true},
   {"type":"CE",  "desc":"Initiating Location"                                          },
   {"type":"CE",  "desc":"Packaging/assembly Location"                                  },
   {"type":"NM",  "desc":"Actual Drug Strength Volume"                                  },
   {"type":"CWE", "desc":"Actual Drug Strength Volume Units"                            },
   {"type":"CWE", "desc":"Dispense To Pharmacy"                                         },
   {"type":"XAD", "desc":"Dispense To Pharmacy Address"                                 },
   {"type":"ID",  "desc":"Pharmacy Order Type"                                          },
   {"type":"CWE", "desc":"Dispense Type"                                                }]},
 "RXE":{
  "desc":"Pharmacy/Treatment Encoded Order",
  "type":"segment",
  "children":[
   {"type":"TQ",  "desc":"Quantity/timing"                                              },
   {"type":"CE",  "desc":"Give Code"                                                     ,"req":true},
   {"type":"NM",  "desc":"Give Amount - Minimum"                                         ,"req":true},
   {"type":"NM",  "desc":"Give Amount - Maximum"                                        },
   {"type":"CE",  "desc":"Give Units"                                                    ,"req":true},
   {"type":"CE",  "desc":"Give Dosage Form"                                             },
   {"type":"CE",  "desc":"Provider's Administration Instructions"                        ,"repeats":true},
   {"type":"LA1", "desc":"Deliver-to Location"                                          },
   {"type":"ID",  "desc":"Substitution Status"                                          },
   {"type":"NM",  "desc":"Dispense Amount"                                              },
   {"type":"CE",  "desc":"Dispense Units"                                               },
   {"type":"NM",  "desc":"Number Of Refills"                                            },
   {"type":"XCN", "desc":"Ordering Provider's Dea Number"                                ,"repeats":true},
   {"type":"XCN", "desc":"Pharmacist/treatment Supplier's Verifier ID"                   ,"repeats":true},
   {"type":"ST",  "desc":"Prescription Number"                                          },
   {"type":"NM",  "desc":"Number Of Refills Remaining"                                  },
   {"type":"NM",  "desc":"Number Of Refills/doses Dispensed"                            },
   {"type":"TS",  "desc":"D/t Of Most Recent Refill Or Dose Dispensed"                  },
   {"type":"CQ",  "desc":"Total Daily Dose"                                             },
   {"type":"ID",  "desc":"Needs Human Review"                                           },
   {"type":"CE",  "desc":"Pharmacy/treatment Supplier's Special Dispensing Instructions" ,"repeats":true},
   {"type":"ST",  "desc":"Give Per (time Unit)"                                         },
   {"type":"ST",  "desc":"Give Rate Amount"                                             },
   {"type":"CE",  "desc":"Give Rate Units"                                              },
   {"type":"NM",  "desc":"Give Strength"                                                },
   {"type":"CE",  "desc":"Give Strength Units"                                          },
   {"type":"CE",  "desc":"Give Indication"                                               ,"repeats":true},
   {"type":"NM",  "desc":"Dispense Package Size"                                        },
   {"type":"CE",  "desc":"Dispense Package Size Unit"                                   },
   {"type":"ID",  "desc":"Dispense Package Method"                                      },
   {"type":"CE",  "desc":"Supplementary Code"                                            ,"repeats":true},
   {"type":"TS",  "desc":"Original Order Date/time"                                     },
   {"type":"NM",  "desc":"Give Drug Strength Volume"                                    },
   {"type":"CWE", "desc":"Give Drug Strength Volume Units"                              },
   {"type":"CWE", "desc":"Controlled Substance Schedule"                                },
   {"type":"ID",  "desc":"Formulary Status"                                             },
   {"type":"CWE", "desc":"Pharmaceutical Substance Alternative"                          ,"repeats":true},
   {"type":"CWE", "desc":"Pharmacy Of Most Recent Fill"                                 },
   {"type":"NM",  "desc":"Initial Dispense Amount"                                      },
   {"type":"CWE", "desc":"Dispensing Pharmacy"                                          },
   {"type":"XAD", "desc":"Dispensing Pharmacy Address"                                  },
   {"type":"PL",  "desc":"Deliver-to Patient Location"                                  },
   {"type":"XAD", "desc":"Deliver-to Address"                                           },
   {"type":"ID",  "desc":"Pharmacy Order Type"                                          }]},
 "RXG":{
  "desc":"Pharmacy/Treatment Give",
  "type":"segment",
  "children":[
   {"type":"NM",  "desc":"Give Sub-id Counter"                                               ,"req":true},
   {"type":"NM",  "desc":"Dispense Sub-id Counter"                                          },
   {"type":"TQ",  "desc":"Quantity/timing"                                                  },
   {"type":"CE",  "desc":"Give Code"                                                         ,"req":true},
   {"type":"NM",  "desc":"Give Amount - Minimum"                                             ,"req":true},
   {"type":"NM",  "desc":"Give Amount - Maximum"                                            },
   {"type":"CE",  "desc":"Give Units"                                                        ,"req":true},
   {"type":"CE",  "desc":"Give Dosage Form"                                                 },
   {"type":"CE",  "desc":"Administration Notes"                                              ,"repeats":true},
   {"type":"ID",  "desc":"Substitution Status"                                              },
   {"type":"LA2", "desc":"Dispense-to Location"                                             },
   {"type":"ID",  "desc":"Needs Human Review"                                               },
   {"type":"CE",  "desc":"Pharmacy/treatment Supplier's Special Administration Instructions" ,"repeats":true},
   {"type":"ST",  "desc":"Give Per (time Unit)"                                             },
   {"type":"ST",  "desc":"Give Rate Amount"                                                 },
   {"type":"CE",  "desc":"Give Rate Units"                                                  },
   {"type":"NM",  "desc":"Give Strength"                                                    },
   {"type":"CE",  "desc":"Give Strength Units"                                              },
   {"type":"ST",  "desc":"Substance Lot Number"                                              ,"repeats":true},
   {"type":"TS",  "desc":"Substance Expiration Date"                                         ,"repeats":true},
   {"type":"CE",  "desc":"Substance Manufacturer Name"                                       ,"repeats":true},
   {"type":"CE",  "desc":"Indication"                                                        ,"repeats":true},
   {"type":"NM",  "desc":"Give Drug Strength Volume"                                        },
   {"type":"CWE", "desc":"Give Drug Strength Volume Units"                                  },
   {"type":"CWE", "desc":"Give Barcode Identifier"                                          },
   {"type":"ID",  "desc":"Pharmacy Order Type"                                              }]},
 "RXO":{
  "desc":"Pharmacy/Treatment Order",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Requested Give Code"                        },
   {"type":"NM",  "desc":"Requested Give Amount - Minimum"            },
   {"type":"NM",  "desc":"Requested Give Amount - Maximum"            },
   {"type":"CE",  "desc":"Requested Give Units"                       },
   {"type":"CE",  "desc":"Requested Dosage Form"                      },
   {"type":"CE",  "desc":"Provider's Pharmacy/treatment Instructions"  ,"repeats":true},
   {"type":"CE",  "desc":"Provider's Administration Instructions"      ,"repeats":true},
   {"type":"LA1", "desc":"Deliver-to Location"                        },
   {"type":"ID",  "desc":"Allow Substitutions"                        },
   {"type":"CE",  "desc":"Requested Dispense Code"                    },
   {"type":"NM",  "desc":"Requested Dispense Amount"                  },
   {"type":"CE",  "desc":"Requested Dispense Units"                   },
   {"type":"NM",  "desc":"Number Of Refills"                          },
   {"type":"XCN", "desc":"Ordering Provider's Dea Number"              ,"repeats":true},
   {"type":"XCN", "desc":"Pharmacist/treatment Supplier's Verifier ID" ,"repeats":true},
   {"type":"ID",  "desc":"Needs Human Review"                         },
   {"type":"ST",  "desc":"Requested Give Per (time Unit)"             },
   {"type":"NM",  "desc":"Requested Give Strength"                    },
   {"type":"CE",  "desc":"Requested Give Strength Units"              },
   {"type":"CE",  "desc":"Indication"                                  ,"repeats":true},
   {"type":"ST",  "desc":"Requested Give Rate Amount"                 },
   {"type":"CE",  "desc":"Requested Give Rate Units"                  },
   {"type":"CQ",  "desc":"Total Daily Dose"                           },
   {"type":"CE",  "desc":"Supplementary Code"                          ,"repeats":true},
   {"type":"NM",  "desc":"Requested Drug Strength Volume"             },
   {"type":"CWE", "desc":"Requested Drug Strength Volume Units"       },
   {"type":"ID",  "desc":"Pharmacy Order Type"                        },
   {"type":"NM",  "desc":"Dispensing Interval"                        }]},
 "RXR":{
  "desc":"Pharmacy/Treatment Route",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Route"                        ,"req":true},
   {"type":"CWE", "desc":"Administration Site"         },
   {"type":"CE",  "desc":"Administration Device"       },
   {"type":"CWE", "desc":"Administration Method"       },
   {"type":"CE",  "desc":"Routing Instruction"         },
   {"type":"CWE", "desc":"Administration Site Modifier"}]},
 "RX_ADMIN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"          ,"req":true}]},
 "RX_ADMINISTRATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"         }]},
 "RX_ADMINISTRATION_PEX_CAUSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"         }]},
 "RX_ADMIN_STUDY_PHARM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"          ,"req":true}]},
 "RX_ADMIN_STUDY_PHARM_STUDY_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"          ,"req":true}]},
 "RX_ADMIN_STUDY_PHARM_STUDY_SCHEDULE_STUDY_PHASE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXA", "desc":"Pharmacy/Treatment Administration" ,"req":true},
   {"type":"RXR", "desc":"Pharmacy/Treatment Route"          ,"req":true}]},
 "RX_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                 "desc":"Pharmacy/Treatment Encoded Order" ,"req":true},
   {"type":"TIMING_QTY_RX_ORDER", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"RXR",                 "desc":"Pharmacy/Treatment Route"         ,"repeats":true}]},
 "RX_ORDER_PEX_CAUSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXE",                           "desc":"Pharmacy/Treatment Encoded Order" ,"req":true},
   {"type":"TIMING_QTY_RX_ORDER_PEX_CAUSE", "desc":""                                 ,"req":true ,"repeats":true},
   {"type":"RXR",                           "desc":"Pharmacy/Treatment Route"         ,"repeats":true}]},
 "SAC":{
  "desc":"Specimen Container Detail",
  "type":"segment",
  "children":[
   {"type":"EI",  "desc":"External Accession Identifier"        },
   {"type":"EI",  "desc":"Accession Identifier"                 },
   {"type":"EI",  "desc":"Container Identifier"                 },
   {"type":"EI",  "desc":"Primary (parent) Container Identifier"},
   {"type":"EI",  "desc":"Equipment Container Identifier"       },
   {"type":"SPS", "desc":"Specimen Source"                      },
   {"type":"TS",  "desc":"Registration Date/time"               },
   {"type":"CE",  "desc":"Container Status"                     },
   {"type":"CE",  "desc":"Carrier Type"                         },
   {"type":"EI",  "desc":"Carrier Identifier"                   },
   {"type":"NA",  "desc":"Position In Carrier"                  },
   {"type":"CE",  "desc":"Tray Type - Sac"                      },
   {"type":"EI",  "desc":"Tray Identifier"                      },
   {"type":"NA",  "desc":"Position In Tray"                     },
   {"type":"CE",  "desc":"Location"                              ,"repeats":true},
   {"type":"NM",  "desc":"Container Height"                     },
   {"type":"NM",  "desc":"Container Diameter"                   },
   {"type":"NM",  "desc":"Barrier Delta"                        },
   {"type":"NM",  "desc":"Bottom Delta"                         },
   {"type":"CE",  "desc":"Container Height/diameter/delta Units"},
   {"type":"NM",  "desc":"Container Volume"                     },
   {"type":"NM",  "desc":"Available Specimen Volume"            },
   {"type":"NM",  "desc":"Initial Specimen Volume"              },
   {"type":"CE",  "desc":"Volume Units"                         },
   {"type":"CE",  "desc":"Separator Type"                       },
   {"type":"CE",  "desc":"Cap Type"                             },
   {"type":"CWE", "desc":"Additive"                              ,"repeats":true},
   {"type":"CE",  "desc":"Specimen Component"                   },
   {"type":"SN",  "desc":"Dilution Factor"                      },
   {"type":"CE",  "desc":"Treatment"                            },
   {"type":"SN",  "desc":"Temperature"                          },
   {"type":"NM",  "desc":"Hemolysis Index"                      },
   {"type":"CE",  "desc":"Hemolysis Index Units"                },
   {"type":"NM",  "desc":"Lipemia Index"                        },
   {"type":"CE",  "desc":"Lipemia Index Units"                  },
   {"type":"NM",  "desc":"Icterus Index"                        },
   {"type":"CE",  "desc":"Icterus Index Units"                  },
   {"type":"NM",  "desc":"Fibrin Index"                         },
   {"type":"CE",  "desc":"Fibrin Index Units"                   },
   {"type":"CE",  "desc":"System Induced Contaminants"           ,"repeats":true},
   {"type":"CE",  "desc":"Drug Interference"                     ,"repeats":true},
   {"type":"CE",  "desc":"Artificial Blood"                     },
   {"type":"CWE", "desc":"Special Handling Code"                 ,"repeats":true},
   {"type":"CE",  "desc":"Other Environmental Factors"           ,"repeats":true}]},
 "SAD":{
  "desc":"Street Address",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Street Or Mailing Address"},
   {"type":"ST", "desc":"Street Name"              },
   {"type":"ST", "desc":"Dwelling Number"          }]},
 "SCH":{
  "desc":"Scheduling Activity Information",
  "type":"segment",
  "children":[
   {"type":"EI",  "desc":"Placer Appointment ID"       },
   {"type":"EI",  "desc":"Filler Appointment ID"       },
   {"type":"NM",  "desc":"Occurrence Number"           },
   {"type":"EI",  "desc":"Placer Group Number"         },
   {"type":"CE",  "desc":"Schedule ID"                 },
   {"type":"CE",  "desc":"Event Reason"                 ,"req":true},
   {"type":"CE",  "desc":"Appointment Reason"          },
   {"type":"CE",  "desc":"Appointment Type"            },
   {"type":"NM",  "desc":"Appointment Duration"        },
   {"type":"CE",  "desc":"Appointment Duration Units"  },
   {"type":"TQ",  "desc":"Appointment Timing Quantity"  ,"repeats":true},
   {"type":"XCN", "desc":"Placer Contact Person"        ,"repeats":true},
   {"type":"XTN", "desc":"Placer Contact Phone Number" },
   {"type":"XAD", "desc":"Placer Contact Address"       ,"repeats":true},
   {"type":"PL",  "desc":"Placer Contact Location"     },
   {"type":"XCN", "desc":"Filler Contact Person"        ,"req":true ,"repeats":true},
   {"type":"XTN", "desc":"Filler Contact Phone Number" },
   {"type":"XAD", "desc":"Filler Contact Address"       ,"repeats":true},
   {"type":"PL",  "desc":"Filler Contact Location"     },
   {"type":"XCN", "desc":"Entered By Person"            ,"req":true ,"repeats":true},
   {"type":"XTN", "desc":"Entered By Phone Number"      ,"repeats":true},
   {"type":"PL",  "desc":"Entered By Location"         },
   {"type":"EI",  "desc":"Parent Placer Appointment ID"},
   {"type":"EI",  "desc":"Parent Filler Appointment ID"},
   {"type":"CE",  "desc":"Filler Status Code"          },
   {"type":"EI",  "desc":"Placer Order Number"          ,"repeats":true},
   {"type":"EI",  "desc":"Filler Order Number"          ,"repeats":true}]},
 "SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE",   "desc":""                               },
   {"type":"RESOURCES_SCHEDULE", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS01",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS01", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS02",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS02", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS03",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS03", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS04",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS04", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS05",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS05", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS06",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS06", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS07",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS07", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS08",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS08", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS09",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS09", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS10",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS10", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCHEDULE_SRRS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SCH",                       "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",                       "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",                       "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SCHEDULE_SRRS11",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SCHEDULE_SRRS11", "desc":""                                ,"req":true ,"repeats":true}]},
 "SCV":{
  "desc":"Scheduling Class Value Pair",
  "type":"composite",
  "children":[
   {"type":"CWE", "desc":"Parameter Class"},
   {"type":"ST",  "desc":"Parameter Value"}]},
 "SERVICE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"}]},
 "SERVICE_RESOURCES_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SCHEDULE_SRRS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SIUS26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS01":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SERVICE_RESOURCES_SRMS11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AIS", "desc":"Appointment Information" ,"req":true},
   {"type":"APR", "desc":"Appointment Preferences"},
   {"type":"NTE", "desc":"Notes And Comments"      ,"repeats":true}]},
 "SFT":{
  "desc":"Software Segment",
  "type":"segment",
  "children":[
   {"type":"XON", "desc":"Software Vendor Organization"                 ,"req":true},
   {"type":"ST",  "desc":"Software Certified Version Or Release Number" ,"req":true},
   {"type":"ST",  "desc":"Software Product Name"                        ,"req":true},
   {"type":"ST",  "desc":"Software Binary ID"                           ,"req":true},
   {"type":"TX",  "desc":"Software Product Information"                },
   {"type":"TS",  "desc":"Software Install Date"                       }]},
 "SI":{
  "desc":"Sequence ID",
  "type":"composite"
 },
 "SID":{
  "desc":"Substance Identifier",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Application / Method Identifier"  },
   {"type":"ST", "desc":"Substance Lot Number"             },
   {"type":"ST", "desc":"Substance Container Identifier"   },
   {"type":"CE", "desc":"Substance Manufacturer Identifier"}]},
 "SIUS12":{
  "desc":"Notification of new appointment booking",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",            "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",            "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",            "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS12", "desc":""                                ,"repeats":true},
   {"type":"RESOURCES",      "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS13":{
  "desc":"Notification of appointment rescheduling",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS13",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS13", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS14":{
  "desc":"Notification of appointment modification",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS14",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS14", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS15":{
  "desc":"Notification of appointment cancellation",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS15",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS15", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS16":{
  "desc":"Notification of appointment discontinuation",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS16",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS16", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS17":{
  "desc":"Notification of appointment deletion",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS17",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS17", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS18":{
  "desc":"Notification of addition of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS18",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS18", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS19":{
  "desc":"Notification of modification of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS19",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS19", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS20":{
  "desc":"Notification of cancellation of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS20",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS20", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS21":{
  "desc":"Notification of discontinuation of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS21",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS21", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS22":{
  "desc":"Notification of deletion of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS22",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS22", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS23":{
  "desc":"Notification of blocked schedule time slot(s)",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS23",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS23", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS24":{
  "desc":"Notification of opened (unblocked) schedule time slot(s)",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS24",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS24", "desc":""                                ,"req":true ,"repeats":true}]},
 "SIUS26":{
  "desc":"Notification that patient did not show up for scheduled appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                  ,"req":true},
   {"type":"SCH",              "desc":"Scheduling Activity Information" ,"req":true},
   {"type":"TQ1",              "desc":"Timing/Quantity"                 ,"repeats":true},
   {"type":"NTE",              "desc":"Notes And Comments"              ,"repeats":true},
   {"type":"PATIENT_SIUS26",   "desc":""                                ,"repeats":true},
   {"type":"RESOURCES_SIUS26", "desc":""                                ,"req":true ,"repeats":true}]},
 "SN":{
  "desc":"Structured Numeric",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Comparator"      },
   {"type":"NM", "desc":"Num1"            },
   {"type":"ST", "desc":"Separator/suffix"},
   {"type":"NM", "desc":"Num2"            }]},
 "SPD":{
  "desc":"Specialty Description",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Specialty Name"       },
   {"type":"ST", "desc":"Governing Board"      },
   {"type":"ID", "desc":"Eligible Or Certified"},
   {"type":"DT", "desc":"Date Of Certification"}]},
 "SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",       "desc":"Specimen"           ,"req":true},
   {"type":"OBX",       "desc":"Observation/Result" ,"repeats":true},
   {"type":"CONTAINER", "desc":""                   ,"repeats":true}]},
 "SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"SPM", "desc":"Specimen"                  ,"repeats":true}]},
 "SPECIMEN_CONTAINER_COMMAND_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"SPM", "desc":"Specimen"                  ,"repeats":true}]},
 "SPECIMEN_CONTAINER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC",                      "desc":"Specimen Container Detail" ,"req":true},
   {"type":"ORDER_SPECIMEN_CONTAINER", "desc":""                          ,"req":true ,"repeats":true}]},
 "SPECIMEN_CONTAINER_SPECIMEN_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC",                               "desc":"Specimen Container Detail" ,"req":true},
   {"type":"ORDER_SPECIMEN_CONTAINER_SPECIMEN", "desc":""                          ,"repeats":true}]},
 "SPECIMEN_CONTAINER_SSRU04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC", "desc":"Specimen Container Detail" ,"req":true},
   {"type":"SPM", "desc":"Specimen"                  ,"repeats":true}]},
 "SPECIMEN_CONTAINER_SSUU03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SAC",                         "desc":"Specimen Container Detail" ,"req":true},
   {"type":"OBX",                         "desc":"Observation/Result"        ,"repeats":true},
   {"type":"SPECIMEN_SPECIMEN_CONTAINER", "desc":""                          ,"repeats":true}]},
 "SPECIMEN_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",                "desc":"Specimen"           ,"req":true},
   {"type":"OBX",                "desc":"Observation/Result" ,"repeats":true},
   {"type":"CONTAINER_SPECIMEN", "desc":""                   ,"repeats":true}]},
 "SPECIMEN_OBSERVATION_REQUEST_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"                  ,"req":true},
   {"type":"SAC", "desc":"Specimen Container Detail" ,"repeats":true}]},
 "SPECIMEN_OBSERVATION_REQUEST_ORDER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"                  ,"req":true},
   {"type":"SAC", "desc":"Specimen Container Detail" ,"repeats":true}]},
 "SPECIMEN_OMLO33":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",            "desc":"Specimen"                  ,"req":true},
   {"type":"OBX",            "desc":"Observation/Result"        ,"repeats":true},
   {"type":"SAC",            "desc":"Specimen Container Detail" ,"repeats":true},
   {"type":"ORDER_SPECIMEN", "desc":""                          ,"req":true ,"repeats":true}]},
 "SPECIMEN_OMLO35":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",                         "desc":"Specimen"           ,"req":true},
   {"type":"OBX",                         "desc":"Observation/Result" ,"repeats":true},
   {"type":"SPECIMEN_CONTAINER_SPECIMEN", "desc":""                   ,"req":true ,"repeats":true}]},
 "SPECIMEN_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"                  ,"req":true},
   {"type":"SAC", "desc":"Specimen Container Detail" ,"repeats":true}]},
 "SPECIMEN_ORDER_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"           ,"req":true},
   {"type":"OBX", "desc":"Observation/Result" ,"repeats":true}]},
 "SPECIMEN_ORDER_OULR24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",                      "desc":"Specimen"           ,"req":true},
   {"type":"OBX",                      "desc":"Observation/Result" ,"repeats":true},
   {"type":"CONTAINER_SPECIMEN_ORDER", "desc":""                   ,"repeats":true}]},
 "SPECIMEN_OULR22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",                       "desc":"Specimen"           ,"req":true},
   {"type":"OBX",                       "desc":"Observation/Result" ,"repeats":true},
   {"type":"CONTAINER_SPECIMEN_OULR22", "desc":""                   ,"repeats":true},
   {"type":"ORDER_SPECIMEN_OULR22",     "desc":""                   ,"req":true ,"repeats":true}]},
 "SPECIMEN_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",                    "desc":"Specimen"                  ,"req":true},
   {"type":"OBX",                    "desc":"Observation/Result"        ,"repeats":true},
   {"type":"SAC",                    "desc":"Specimen Container Detail" ,"repeats":true},
   {"type":"ORDER_SPECIMEN_PATIENT", "desc":""                          ,"repeats":true}]},
 "SPECIMEN_PATIENT_ORLO36":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM",                                 "desc":"Specimen"           ,"req":true},
   {"type":"OBX",                                 "desc":"Observation/Result" ,"repeats":true},
   {"type":"SPECIMEN_CONTAINER_SPECIMEN_PATIENT", "desc":""                   ,"req":true ,"repeats":true}]},
 "SPECIMEN_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"           ,"req":true},
   {"type":"OBX", "desc":"Observation/Result" ,"repeats":true}]},
 "SPECIMEN_SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"           ,"req":true},
   {"type":"OBX", "desc":"Observation/Result" ,"repeats":true}]},
 "SPM":{
  "desc":"Specimen",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID _ Spm"                 },
   {"type":"EIP", "desc":"Specimen ID"                  },
   {"type":"EIP", "desc":"Specimen Parent Ids"           ,"repeats":true},
   {"type":"CWE", "desc":"Specimen Type"                 ,"req":true},
   {"type":"CWE", "desc":"Specimen Type Modifier"        ,"repeats":true},
   {"type":"CWE", "desc":"Specimen Additives"            ,"repeats":true},
   {"type":"CWE", "desc":"Specimen Collection Method"   },
   {"type":"CWE", "desc":"Specimen Source Site"         },
   {"type":"CWE", "desc":"Specimen Source Site Modifier" ,"repeats":true},
   {"type":"CWE", "desc":"Specimen Collection Site"     },
   {"type":"CWE", "desc":"Specimen Role"                 ,"repeats":true},
   {"type":"CQ",  "desc":"Specimen Collection Amount"   },
   {"type":"NM",  "desc":"Grouped Specimen Count"       },
   {"type":"ST",  "desc":"Specimen Description"          ,"repeats":true},
   {"type":"CWE", "desc":"Specimen Handling Code"        ,"repeats":true},
   {"type":"CWE", "desc":"Specimen Risk Code"            ,"repeats":true},
   {"type":"DR",  "desc":"Specimen Collection Date/time"},
   {"type":"TS",  "desc":"Specimen Received Date/time"  },
   {"type":"TS",  "desc":"Specimen Expiration Date/time"},
   {"type":"ID",  "desc":"Specimen Availability"        },
   {"type":"CWE", "desc":"Specimen Reject Reason"        ,"repeats":true},
   {"type":"CWE", "desc":"Specimen Quality"             },
   {"type":"CWE", "desc":"Specimen Appropriateness"     },
   {"type":"CWE", "desc":"Specimen Condition"            ,"repeats":true},
   {"type":"CQ",  "desc":"Specimen Current Quantity"    },
   {"type":"NM",  "desc":"Number Of Specimen Containers"},
   {"type":"CWE", "desc":"Container Type"               },
   {"type":"CWE", "desc":"Container Condition"          },
   {"type":"CWE", "desc":"Specimen Child Role"          }]},
 "SPR":{
  "desc":"Stored Procedure Request Definition",
  "type":"segment",
  "children":[
   {"type":"ST",  "desc":"Query Tag"                 },
   {"type":"ID",  "desc":"Query/response Format Code" ,"req":true},
   {"type":"CE",  "desc":"Stored Procedure Name"      ,"req":true},
   {"type":"QIP", "desc":"Input Parameter List"       ,"repeats":true}]},
 "SPS":{
  "desc":"Specimen Source",
  "type":"composite",
  "children":[
   {"type":"CWE", "desc":"Specimen Source Name Or Code"   },
   {"type":"CWE", "desc":"Additives"                      },
   {"type":"TX",  "desc":"Specimen Collection Method"     },
   {"type":"CWE", "desc":"Body Site"                      },
   {"type":"CWE", "desc":"Site Modifier"                  },
   {"type":"CWE", "desc":"Collection Method Modifier Code"},
   {"type":"CWE", "desc":"Specimen Role"                  }]},
 "SQMS25":{
  "desc":"Schedule query message and response",
  "type":"message",
  "children":[
   {"type":"MSH",     "desc":"Message Header"              ,"req":true},
   {"type":"QRD",     "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",     "desc":"Original Style Query Filter"},
   {"type":"REQUEST", "desc":""                           },
   {"type":"DSC",     "desc":"Continuation Pointer"       }]},
 "SQRS25":{
  "desc":"Schedule query message and response",
  "type":"message",
  "children":[
   {"type":"MSH",      "desc":"Message Header"         ,"req":true},
   {"type":"MSA",      "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",      "desc":"Error"                  ,"repeats":true},
   {"type":"QAK",      "desc":"Query Acknowledgment"   ,"req":true},
   {"type":"SCHEDULE", "desc":""                       ,"repeats":true},
   {"type":"DSC",      "desc":"Continuation Pointer"  }]},
 "SRMS01":{
  "desc":"Request new appointment booking",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS01",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS01", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS02":{
  "desc":"Request appointment rescheduling",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS02",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS02", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS03":{
  "desc":"Request appointment modification",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS03",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS03", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS04":{
  "desc":"Request appointment cancellation",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS04",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS04", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS05":{
  "desc":"Request appointment discontinuation",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS05",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS05", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS06":{
  "desc":"Request appointment deletion",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS06",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS06", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS07":{
  "desc":"Request addition of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS07",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS07", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS08":{
  "desc":"Request modification of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS08",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS08", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS09":{
  "desc":"Request cancellation of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS09",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS09", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS10":{
  "desc":"Request discontinuation of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS10",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS10", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRMS11":{
  "desc":"Request deletion of service/resource on appointment",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"          ,"req":true},
   {"type":"ARQ",              "desc":"Appointment Request"     ,"req":true},
   {"type":"APR",              "desc":"Appointment Preferences"},
   {"type":"NTE",              "desc":"Notes And Comments"      ,"repeats":true},
   {"type":"PATIENT_SRMS11",   "desc":""                        ,"repeats":true},
   {"type":"RESOURCES_SRMS11", "desc":""                        ,"req":true ,"repeats":true}]},
 "SRRS01":{
  "desc":"Request new appointment booking - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS01", "desc":""                      }]},
 "SRRS02":{
  "desc":"Request appointment rescheduling - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS02", "desc":""                      }]},
 "SRRS03":{
  "desc":"Request appointment modification - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS03", "desc":""                      }]},
 "SRRS04":{
  "desc":"Request appointment cancellation - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS04", "desc":""                      }]},
 "SRRS05":{
  "desc":"Request appointment discontinuation - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS05", "desc":""                      }]},
 "SRRS06":{
  "desc":"Request appointment deletion - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS06", "desc":""                      }]},
 "SRRS07":{
  "desc":"Request addition of service/resource on appointment - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS07", "desc":""                      }]},
 "SRRS08":{
  "desc":"Request modification of service/resource on appointment - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS08", "desc":""                      }]},
 "SRRS09":{
  "desc":"Request cancellation of service/resource on appointment - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS09", "desc":""                      }]},
 "SRRS10":{
  "desc":"Request discontinuation of service/resource on appointment - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS10", "desc":""                      }]},
 "SRRS11":{
  "desc":"Request deletion of service/resource on appointment - response",
  "type":"message",
  "children":[
   {"type":"MSH",             "desc":"Message Header"         ,"req":true},
   {"type":"MSA",             "desc":"Message Acknowledgment" ,"req":true},
   {"type":"ERR",             "desc":"Error"                  ,"repeats":true},
   {"type":"SCHEDULE_SRRS11", "desc":""                      }]},
 "SRT":{
  "desc":"Sort Order",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Sort-by Field"},
   {"type":"ID", "desc":"Sequencing"   }]},
 "SSRU04":{
  "desc":"Specimen status request",
  "type":"message",
  "children":[
   {"type":"MSH",                       "desc":"Message Header"   ,"req":true},
   {"type":"SFT",                       "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU",                       "desc":"Equipment Detail" ,"req":true},
   {"type":"SPECIMEN_CONTAINER_SSRU04", "desc":""                 ,"req":true ,"repeats":true},
   {"type":"ROL",                       "desc":"Role"            }]},
 "SSUU03":{
  "desc":"Specimen status update",
  "type":"message",
  "children":[
   {"type":"MSH",                       "desc":"Message Header"   ,"req":true},
   {"type":"SFT",                       "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU",                       "desc":"Equipment Detail" ,"req":true},
   {"type":"SPECIMEN_CONTAINER_SSUU03", "desc":""                 ,"req":true ,"repeats":true},
   {"type":"ROL",                       "desc":"Role"            }]},
 "ST":{
  "desc":"String Data",
  "type":"composite"
 },
 "STAFF":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"STF", "desc":"Staff Identification"           ,"req":true},
   {"type":"PRA", "desc":"Practitioner Detail"            ,"repeats":true},
   {"type":"ORG", "desc":"Practitioner Organization Unit" ,"repeats":true},
   {"type":"AFF", "desc":"Professional Affiliation"       ,"repeats":true},
   {"type":"LAN", "desc":"Language Detail"                ,"repeats":true},
   {"type":"EDU", "desc":"Educational Detail"             ,"repeats":true},
   {"type":"CER", "desc":"Certificate Detail"             ,"repeats":true}]},
 "STF":{
  "desc":"Staff Identification",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Primary Key Value - Stf"           },
   {"type":"CX",  "desc":"Staff Identifier List"              ,"repeats":true},
   {"type":"XPN", "desc":"Staff Name"                         ,"repeats":true},
   {"type":"IS",  "desc":"Staff Type"                         ,"repeats":true},
   {"type":"IS",  "desc":"Administrative Sex"                },
   {"type":"TS",  "desc":"Date/time Of Birth"                },
   {"type":"ID",  "desc":"Active/inactive Flag"              },
   {"type":"CE",  "desc":"Department"                         ,"repeats":true},
   {"type":"CE",  "desc":"Hospital Service - Stf"             ,"repeats":true},
   {"type":"XTN", "desc":"Phone"                              ,"repeats":true},
   {"type":"XAD", "desc":"Office/home Address/birthplace"     ,"repeats":true},
   {"type":"DIN", "desc":"Institution Activation Date"        ,"repeats":true},
   {"type":"DIN", "desc":"Institution Inactivation Date"      ,"repeats":true},
   {"type":"CE",  "desc":"Backup Person ID"                   ,"repeats":true},
   {"type":"ST",  "desc":"E-mail Address"                     ,"repeats":true},
   {"type":"CE",  "desc":"Preferred Method Of Contact"       },
   {"type":"CE",  "desc":"Marital Status"                    },
   {"type":"ST",  "desc":"Job Title"                         },
   {"type":"JCC", "desc":"Job Code/class"                    },
   {"type":"CE",  "desc":"Employment Status Code"            },
   {"type":"ID",  "desc":"Additional Insured On Auto"        },
   {"type":"DLN", "desc":"Driver's License Number - Staff"   },
   {"type":"ID",  "desc":"Copy Auto Ins"                     },
   {"type":"DT",  "desc":"Auto Ins. Expires"                 },
   {"type":"DT",  "desc":"Date Last Dmv Review"              },
   {"type":"DT",  "desc":"Date Next Dmv Review"              },
   {"type":"CE",  "desc":"Race"                              },
   {"type":"CE",  "desc":"Ethnic Group"                      },
   {"type":"ID",  "desc":"Re-activation Approval Indicator"  },
   {"type":"CE",  "desc":"Citizenship"                        ,"repeats":true},
   {"type":"TS",  "desc":"Death Date And Time"               },
   {"type":"ID",  "desc":"Death Indicator"                   },
   {"type":"CWE", "desc":"Institution Relationship Type Code"},
   {"type":"DR",  "desc":"Institution Relationship Period"   },
   {"type":"DT",  "desc":"Expected Return Date"              },
   {"type":"CWE", "desc":"Cost Center Code"                   ,"repeats":true},
   {"type":"ID",  "desc":"Generic Classification Indicator"  },
   {"type":"CWE", "desc":"Inactive Reason Code"              }]},
 "STUDY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "STUDY_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",        "desc":"Common Order"       },
   {"type":"OBR",        "desc":"Observation Request" ,"req":true},
   {"type":"TIMING_QTY", "desc":""                    ,"repeats":true},
   {"type":"OBX",        "desc":"Observation/Result"  ,"req":true ,"repeats":true}]},
 "STUDY_OBSERVATION_STUDY_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                          "desc":"Common Order"       },
   {"type":"OBR",                          "desc":"Observation Request" ,"req":true},
   {"type":"TIMING_QTY_STUDY_OBSERVATION", "desc":""                    ,"repeats":true},
   {"type":"OBX",                          "desc":"Observation/Result"  ,"req":true ,"repeats":true}]},
 "STUDY_OBSERVATION_STUDY_SCHEDULE_STUDY_PHASE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                         "desc":"Common Order"       },
   {"type":"OBR",                                         "desc":"Observation Request" ,"req":true},
   {"type":"TIMING_QTY_STUDY_OBSERVATION_STUDY_SCHEDULE", "desc":""                    ,"repeats":true},
   {"type":"OBX",                                         "desc":"Observation/Result"  ,"req":true ,"repeats":true}]},
 "STUDY_OBSERVATION_STUDY_SCHEDULE_STUDY_PHASE_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                                     "desc":"Common Order"       },
   {"type":"OBR",                                                     "desc":"Observation Request" ,"req":true},
   {"type":"TIMING_QTY_STUDY_OBSERVATION_STUDY_SCHEDULE_STUDY_PHASE", "desc":""                    ,"repeats":true},
   {"type":"OBX",                                                     "desc":"Observation/Result"  ,"req":true ,"repeats":true}]},
 "STUDY_PEX_CAUSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSR", "desc":"Clinical Study Registration" ,"req":true},
   {"type":"CSP", "desc":"Clinical Study Phase"        ,"repeats":true}]},
 "STUDY_PHARM":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",      "desc":"Common Order"},
   {"type":"RX_ADMIN", "desc":""             ,"req":true ,"repeats":true}]},
 "STUDY_PHARM_STUDY_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                  "desc":"Common Order"},
   {"type":"RX_ADMIN_STUDY_PHARM", "desc":""             ,"req":true ,"repeats":true}]},
 "STUDY_PHARM_STUDY_SCHEDULE_STUDY_PHASE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                 "desc":"Common Order"},
   {"type":"RX_ADMIN_STUDY_PHARM_STUDY_SCHEDULE", "desc":""             ,"req":true ,"repeats":true}]},
 "STUDY_PHARM_STUDY_SCHEDULE_STUDY_PHASE_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"ORC",                                             "desc":"Common Order"},
   {"type":"RX_ADMIN_STUDY_PHARM_STUDY_SCHEDULE_STUDY_PHASE", "desc":""             ,"req":true ,"repeats":true}]},
 "STUDY_PHASE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSP",            "desc":"Clinical Study Phase"},
   {"type":"STUDY_SCHEDULE", "desc":""                     ,"req":true ,"repeats":true}]},
 "STUDY_PHASE_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSP",                        "desc":"Clinical Study Phase"},
   {"type":"STUDY_SCHEDULE_STUDY_PHASE", "desc":""                     ,"req":true ,"repeats":true}]},
 "STUDY_PHASE_PATIENT_CSUC11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSP",                                "desc":"Clinical Study Phase"},
   {"type":"STUDY_SCHEDULE_STUDY_PHASE_PATIENT", "desc":""                     ,"req":true ,"repeats":true}]},
 "STUDY_PHASE_PATIENT_CSUC12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSP",                                       "desc":"Clinical Study Phase"},
   {"type":"STUDY_SCHEDULE_STUDY_PHASE_PATIENT_CSUC12", "desc":""                     ,"req":true ,"repeats":true}]},
 "STUDY_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSS",               "desc":"Clinical Study Data Schedule Segment"},
   {"type":"STUDY_OBSERVATION", "desc":""                                     ,"req":true ,"repeats":true},
   {"type":"STUDY_PHARM",       "desc":""                                     ,"req":true ,"repeats":true}]},
 "STUDY_SCHEDULE_STUDY_PHASE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSS",                              "desc":"Clinical Study Data Schedule Segment"},
   {"type":"STUDY_OBSERVATION_STUDY_SCHEDULE", "desc":""                                     ,"req":true ,"repeats":true},
   {"type":"STUDY_PHARM_STUDY_SCHEDULE",       "desc":""                                     ,"req":true ,"repeats":true}]},
 "STUDY_SCHEDULE_STUDY_PHASE_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSS",                                          "desc":"Clinical Study Data Schedule Segment"},
   {"type":"STUDY_OBSERVATION_STUDY_SCHEDULE_STUDY_PHASE", "desc":""                                     ,"req":true ,"repeats":true},
   {"type":"STUDY_PHARM_STUDY_SCHEDULE_STUDY_PHASE",       "desc":""                                     ,"req":true ,"repeats":true}]},
 "STUDY_SCHEDULE_STUDY_PHASE_PATIENT_CSUC12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"CSS",                                                  "desc":"Clinical Study Data Schedule Segment"},
   {"type":"STUDY_OBSERVATION_STUDY_SCHEDULE_STUDY_PHASE_PATIENT", "desc":""                                     ,"req":true ,"repeats":true},
   {"type":"STUDY_PHARM_STUDY_SCHEDULE_STUDY_PHASE_PATIENT",       "desc":""                                     ,"req":true ,"repeats":true}]},
 "SURP09":{
  "desc":"Summary product experience report",
  "type":"message",
  "children":[
   {"type":"MSH",      "desc":"Message Header" ,"req":true},
   {"type":"FACILITY", "desc":""               ,"req":true ,"repeats":true}]},
 "TCC":{
  "desc":"Test Code Configuration",
  "type":"segment",
  "children":[
   {"type":"CE",  "desc":"Universal Service Identifier"               ,"req":true},
   {"type":"EI",  "desc":"Test Application Identifier"                ,"req":true},
   {"type":"SPS", "desc":"Specimen Source"                           },
   {"type":"SN",  "desc":"Auto-dilution Factor Default"              },
   {"type":"SN",  "desc":"Rerun Dilution Factor Default"             },
   {"type":"SN",  "desc":"Pre-dilution Factor Default"               },
   {"type":"SN",  "desc":"Endogenous Content Of Pre-dilution Diluent"},
   {"type":"NM",  "desc":"Inventory Limits Warning Level"            },
   {"type":"ID",  "desc":"Automatic Rerun Allowed"                   },
   {"type":"ID",  "desc":"Automatic Repeat Allowed"                  },
   {"type":"ID",  "desc":"Automatic Reflex Allowed"                  },
   {"type":"SN",  "desc":"Equipment Dynamic Range"                   },
   {"type":"CE",  "desc":"Units"                                     },
   {"type":"CE",  "desc":"Processing Type"                           }]},
 "TCD":{
  "desc":"Test Code Detail",
  "type":"segment",
  "children":[
   {"type":"CE", "desc":"Universal Service Identifier"               ,"req":true},
   {"type":"SN", "desc":"Auto-dilution Factor"                      },
   {"type":"SN", "desc":"Rerun Dilution Factor"                     },
   {"type":"SN", "desc":"Pre-dilution Factor"                       },
   {"type":"SN", "desc":"Endogenous Content Of Pre-dilution Diluent"},
   {"type":"ID", "desc":"Automatic Repeat Allowed"                  },
   {"type":"ID", "desc":"Reflex Allowed"                            },
   {"type":"CE", "desc":"Analyte Repeat Status"                     }]},
 "TCUU10":{
  "desc":"Automated equipment test code settings update",
  "type":"message",
  "children":[
   {"type":"MSH",                "desc":"Message Header"   ,"req":true},
   {"type":"SFT",                "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU",                "desc":"Equipment Detail" ,"req":true},
   {"type":"TEST_CONFIGURATION", "desc":""                 ,"req":true ,"repeats":true},
   {"type":"ROL",                "desc":"Role"            }]},
 "TCUU11":{
  "desc":"Automated equipment test code settings request",
  "type":"message",
  "children":[
   {"type":"MSH",                       "desc":"Message Header"   ,"req":true},
   {"type":"SFT",                       "desc":"Software Segment" ,"repeats":true},
   {"type":"EQU",                       "desc":"Equipment Detail" ,"req":true},
   {"type":"TEST_CONFIGURATION_TCUU11", "desc":""                 ,"req":true ,"repeats":true},
   {"type":"ROL",                       "desc":"Role"            }]},
 "TEST_CONFIGURATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"               },
   {"type":"TCC", "desc":"Test Code Configuration" ,"req":true ,"repeats":true}]},
 "TEST_CONFIGURATION_TCUU11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"SPM", "desc":"Specimen"               },
   {"type":"TCC", "desc":"Test Code Configuration" ,"req":true ,"repeats":true}]},
 "TIIMING":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIIMING_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIIMING_ORDER_SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT02":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_MDMT11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_QUERY_RESPONSE_RSPZ86":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_QUERY_RESPONSE_RSPZ88":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_COMMON_ORDER_QUERY_RESPONSE_RSPZ90":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_DIET":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_DIET_ORDER_DIET":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ENCODED_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ENCODED_ORDER_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ENCODING":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ENCODING_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ENCODING_ORDER_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ENCODING_ORDER_RESPONSE_RREO26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ENCODING_ORDER_RESPONSE_RSPK31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ORDER_ENCODED":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ENCODED_ORDER_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_GIVE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_GIVE_GIVE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_BTSO31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_OMBO27":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_OMGO19":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_OMIO23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_OMNO07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_OMPO09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_OMSO05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_PATIENT_ORLO22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RASO17":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RDEO11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RDEO25":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RDSO13":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_ORGO20":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_ORIO24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_ORNO08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_ORPO10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_ORSO06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_OSRQ06":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_RRAO18":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_RRDO14":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_RREO12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_RREO26":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_RRGO16":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RESPONSE_RSPK31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_RGVO15":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_SPECIMEN_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_VXRV03":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_ORDER_VXUV04":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_PRIOR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_PRIOR_ORDER_PRIOR":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_PRIOR_ORDER_PRIOR_PRIOR_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_PRIOR_ORDER_PRIOR_PRIOR_RESULT_OBSERVATION_REQUEST":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORDER_CONTAINER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORDER_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORDER_OBSERVATION_OULR21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORDER_OULR24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORDER_SPECIMEN":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORUR30":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORUR31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_ORUR32":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_RX_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_RX_ORDER_PEX_CAUSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_STUDY_OBSERVATION":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_STUDY_OBSERVATION_STUDY_SCHEDULE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QTY_STUDY_OBSERVATION_STUDY_SCHEDULE_STUDY_PHASE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QUANTITY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_QUANTITY_COMMON_ORDER":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_TRAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TIMING_TRAY_ORDER_TRAY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"TQ1", "desc":"Timing/Quantity"              ,"req":true},
   {"type":"TQ2", "desc":"Timing/Quantity Relationship" ,"repeats":true}]},
 "TM":{
  "desc":"Time",
  "type":"composite"
 },
 "TQ":{
  "desc":"Timing Quantity",
  "type":"composite",
  "children":[
   {"type":"CQ",  "desc":"Quantity"           },
   {"type":"RI",  "desc":"Interval"           },
   {"type":"ST",  "desc":"Duration"           },
   {"type":"TS",  "desc":"Start Date/time"    },
   {"type":"TS",  "desc":"End Date/time"      },
   {"type":"ST",  "desc":"Priority"           },
   {"type":"ST",  "desc":"Condition"          },
   {"type":"TX",  "desc":"Text"               },
   {"type":"ID",  "desc":"Conjunction"        },
   {"type":"OSD", "desc":"Order Sequencing"   },
   {"type":"CE",  "desc":"Occurrence Duration"},
   {"type":"NM",  "desc":"Total Occurrences"  }]},
 "TQ1":{
  "desc":"Timing/Quantity",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Tq1"           },
   {"type":"CQ",  "desc":"Quantity"               },
   {"type":"RPT", "desc":"Repeat Pattern"          ,"repeats":true},
   {"type":"TM",  "desc":"Explicit Time"           ,"repeats":true},
   {"type":"CQ",  "desc":"Relative Time And Units" ,"repeats":true},
   {"type":"CQ",  "desc":"Service Duration"       },
   {"type":"TS",  "desc":"Start Date/time"        },
   {"type":"TS",  "desc":"End Date/time"          },
   {"type":"CWE", "desc":"Priority"                ,"repeats":true},
   {"type":"TX",  "desc":"Condition Text"         },
   {"type":"TX",  "desc":"Text Instruction"       },
   {"type":"ID",  "desc":"Conjunction"            },
   {"type":"CQ",  "desc":"Occurrence Duration"    },
   {"type":"NM",  "desc":"Total Occurrence's"     }]},
 "TQ2":{
  "desc":"Timing/Quantity Relationship",
  "type":"segment",
  "children":[
   {"type":"SI", "desc":"Set ID - Tq2"                          },
   {"type":"ID", "desc":"Sequence/results Flag"                 },
   {"type":"EI", "desc":"Related Placer Number"                  ,"repeats":true},
   {"type":"EI", "desc":"Related Filler Number"                  ,"repeats":true},
   {"type":"EI", "desc":"Related Placer Group Number"            ,"repeats":true},
   {"type":"ID", "desc":"Sequence Condition Code"               },
   {"type":"ID", "desc":"Cyclic Entry/exit Indicator"           },
   {"type":"CQ", "desc":"Sequence Condition Time Interval"      },
   {"type":"NM", "desc":"Cyclic Group Maximum Number Of Repeats"},
   {"type":"ID", "desc":"Special Service Request Relationship"  }]},
 "TREATMENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"RXC", "desc":"Pharmacy/Treatment Component Order" ,"req":true ,"repeats":true},
   {"type":"NTE", "desc":"Notes And Comments"                 ,"repeats":true}]},
 "TS":{
  "desc":"Time Stamp",
  "type":"composite",
  "children":[
   {"type":"DTM", "desc":"Time"               },
   {"type":"ID",  "desc":"Degree Of Precision"}]},
 "TX":{
  "desc":"Text Data",
  "type":"composite"
 },
 "TXA":{
  "desc":"Transcription Document Header",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set Id- Txa"                                      ,"req":true},
   {"type":"IS",  "desc":"Document Type"                                    ,"req":true},
   {"type":"ID",  "desc":"Document Content Presentation"                   },
   {"type":"TS",  "desc":"Activity Date/time"                              },
   {"type":"XCN", "desc":"Primary Activity Provider Code/name"              ,"repeats":true},
   {"type":"TS",  "desc":"Origination Date/time"                           },
   {"type":"TS",  "desc":"Transcription Date/time"                         },
   {"type":"TS",  "desc":"Edit Date/time"                                   ,"repeats":true},
   {"type":"XCN", "desc":"Originator Code/name"                             ,"repeats":true},
   {"type":"XCN", "desc":"Assigned Document Authenticator"                  ,"repeats":true},
   {"type":"XCN", "desc":"Transcriptionist Code/name"                       ,"repeats":true},
   {"type":"EI",  "desc":"Unique Document Number"                           ,"req":true},
   {"type":"EI",  "desc":"Parent Document Number"                          },
   {"type":"EI",  "desc":"Placer Order Number"                              ,"repeats":true},
   {"type":"EI",  "desc":"Filler Order Number"                             },
   {"type":"ST",  "desc":"Unique Document File Name"                       },
   {"type":"ID",  "desc":"Document Completion Status"                       ,"req":true},
   {"type":"ID",  "desc":"Document Confidentiality Status"                 },
   {"type":"ID",  "desc":"Document Availability Status"                    },
   {"type":"ID",  "desc":"Document Storage Status"                         },
   {"type":"ST",  "desc":"Document Change Reason"                          },
   {"type":"PPN", "desc":"Authentication Person, Time Stamp"                ,"repeats":true},
   {"type":"XCN", "desc":"Distributed Copies (code And Name Of Recipients)" ,"repeats":true}]},
 "UB1":{
  "desc":"UB82",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Ub1"                   },
   {"type":"NM",  "desc":"Blood Deductible (43)"          },
   {"type":"NM",  "desc":"Blood Furnished-pints Of (40)"  },
   {"type":"NM",  "desc":"Blood Replaced-pints (41)"      },
   {"type":"NM",  "desc":"Blood Not Replaced-pints(42)"   },
   {"type":"NM",  "desc":"Co-insurance Days (25)"         },
   {"type":"IS",  "desc":"Condition Code (35-39)"          ,"repeats":true},
   {"type":"NM",  "desc":"Covered Days - (23)"            },
   {"type":"NM",  "desc":"Non Covered Days - (24)"        },
   {"type":"UVC", "desc":"Value Amount & Code (46-49)"     ,"repeats":true},
   {"type":"NM",  "desc":"Number Of Grace Days (90)"      },
   {"type":"CE",  "desc":"Special Program Indicator (44)" },
   {"type":"CE",  "desc":"Psro/ur Approval Indicator (87)"},
   {"type":"DT",  "desc":"Psro/ur Approved Stay-fm (88)"  },
   {"type":"DT",  "desc":"Psro/ur Approved Stay-to (89)"  },
   {"type":"OCD", "desc":"Occurrence (28-32)"              ,"repeats":true},
   {"type":"CE",  "desc":"Occurrence Span (33)"           },
   {"type":"DT",  "desc":"Occur Span Start Date(33)"      },
   {"type":"DT",  "desc":"Occur Span End Date (33)"       },
   {"type":"ST",  "desc":"Ub-82 Locator 2"                },
   {"type":"ST",  "desc":"Ub-82 Locator 9"                },
   {"type":"ST",  "desc":"Ub-82 Locator 27"               },
   {"type":"ST",  "desc":"Ub-82 Locator 45"               }]},
 "UB2":{
  "desc":"UB92 Data",
  "type":"segment",
  "children":[
   {"type":"SI",  "desc":"Set ID - Ub2"                   },
   {"type":"ST",  "desc":"Co-insurance Days (9)"          },
   {"type":"IS",  "desc":"Condition Code (24-30)"          ,"repeats":true},
   {"type":"ST",  "desc":"Covered Days (7)"               },
   {"type":"ST",  "desc":"Non-covered Days (8)"           },
   {"type":"UVC", "desc":"Value Amount & Code"             ,"repeats":true},
   {"type":"OCD", "desc":"Occurrence Code & Date (32-35)"  ,"repeats":true},
   {"type":"OSP", "desc":"Occurrence Span Code/dates (36)" ,"repeats":true},
   {"type":"ST",  "desc":"Ub92 Locator 2 (state)"          ,"repeats":true},
   {"type":"ST",  "desc":"Ub92 Locator 11 (state)"         ,"repeats":true},
   {"type":"ST",  "desc":"Ub92 Locator 31 (national)"     },
   {"type":"ST",  "desc":"Document Control Number"         ,"repeats":true},
   {"type":"ST",  "desc":"Ub92 Locator 49 (national)"      ,"repeats":true},
   {"type":"ST",  "desc":"Ub92 Locator 56 (state)"         ,"repeats":true},
   {"type":"ST",  "desc":"Ub92 Locator 57 (national)"     },
   {"type":"ST",  "desc":"Ub92 Locator 78 (state)"         ,"repeats":true},
   {"type":"NM",  "desc":"Special Visit Count"            }]},
 "URD":{
  "desc":"Results/Update Definition",
  "type":"segment",
  "children":[
   {"type":"TS",  "desc":"R/u Date/time"              },
   {"type":"ID",  "desc":"Report Priority"            },
   {"type":"XCN", "desc":"R/u Who Subject Definition"  ,"req":true ,"repeats":true},
   {"type":"CE",  "desc":"R/u What Subject Definition" ,"repeats":true},
   {"type":"CE",  "desc":"R/u What Department Code"    ,"repeats":true},
   {"type":"ST",  "desc":"R/u Display/print Locations" ,"repeats":true},
   {"type":"ID",  "desc":"R/u Results Level"          }]},
 "URS":{
  "desc":"Unsolicited Selection",
  "type":"segment",
  "children":[
   {"type":"ST", "desc":"R/u Where Subject Definition"         ,"req":true ,"repeats":true},
   {"type":"TS", "desc":"R/u When Data Start Date/time"       },
   {"type":"TS", "desc":"R/u When Data End Date/time"         },
   {"type":"ST", "desc":"R/u What User Qualifier"              ,"repeats":true},
   {"type":"ST", "desc":"R/u Other Results Subject Definition" ,"repeats":true},
   {"type":"ID", "desc":"R/u Which Date/time Qualifier"        ,"repeats":true},
   {"type":"ID", "desc":"R/u Which Date/time Status Qualifier" ,"repeats":true},
   {"type":"ID", "desc":"R/u Date/time Selection Qualifier"    ,"repeats":true},
   {"type":"TQ", "desc":"R/u Quantity/timing Qualifier"       }]},
 "UVC":{
  "desc":"UB Value Code And Amount",
  "type":"composite",
  "children":[
   {"type":"CNE", "desc":"Value Code"  },
   {"type":"MO",  "desc":"Value Amount"}]},
 "VAR":{
  "desc":"Variance",
  "type":"segment",
  "children":[
   {"type":"EI",  "desc":"Variance Instance ID"      ,"req":true},
   {"type":"TS",  "desc":"Documented Date/time"      ,"req":true},
   {"type":"TS",  "desc":"Stated Variance Date/time"},
   {"type":"XCN", "desc":"Variance Originator"       ,"repeats":true},
   {"type":"CE",  "desc":"Variance Classification"  },
   {"type":"ST",  "desc":"Variance Description"      ,"repeats":true}]},
 "VH":{
  "desc":"Visiting Hours",
  "type":"composite",
  "children":[
   {"type":"ID", "desc":"Start Day Range" },
   {"type":"ID", "desc":"End Day Range"   },
   {"type":"TM", "desc":"Start Hour Range"},
   {"type":"TM", "desc":"End Hour Range"  }]},
 "VID":{
  "desc":"Version Identifier",
  "type":"composite",
  "children":[
   {"type":"ID", "desc":"Version ID"               },
   {"type":"CE", "desc":"Internationalization Code"},
   {"type":"CE", "desc":"International Version ID" }]},
 "VISIT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1",             "desc":"Patient Visit"                   },
   {"type":"PV2",             "desc":"Additional Information"          },
   {"type":"ROL",             "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",             "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",             "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",             "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",             "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",             "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_VISIT", "desc":""                                 ,"repeats":true},
   {"type":"GT1",             "desc":"Guarantor"                        ,"repeats":true},
   {"type":"NK1",             "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"INSURANCE_VISIT", "desc":""                                 ,"repeats":true},
   {"type":"ACC",             "desc":"Accident"                        },
   {"type":"UB1",             "desc":"UB82"                            },
   {"type":"UB2",             "desc":"UB92 Data"                       }]},
 "VISIT_ALLERGY":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_BARP05":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1",                    "desc":"Patient Visit"                   },
   {"type":"PV2",                    "desc":"Additional Information"          },
   {"type":"ROL",                    "desc":"Role"                             ,"repeats":true},
   {"type":"DB1",                    "desc":"Disability"                       ,"repeats":true},
   {"type":"OBX",                    "desc":"Observation/Result"               ,"repeats":true},
   {"type":"AL1",                    "desc":"Patient Allergy Information"      ,"repeats":true},
   {"type":"DG1",                    "desc":"Diagnosis"                        ,"repeats":true},
   {"type":"DRG",                    "desc":"Diagnosis Related Group"         },
   {"type":"PROCEDURE_VISIT_BARP05", "desc":""                                 ,"repeats":true},
   {"type":"GT1",                    "desc":"Guarantor"                        ,"repeats":true},
   {"type":"NK1",                    "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"INSURANCE_VISIT_BARP05", "desc":""                                 ,"repeats":true},
   {"type":"ACC",                    "desc":"Accident"                        },
   {"type":"UB1",                    "desc":"UB82"                            },
   {"type":"UB2",                    "desc":"UB92 Data"                       },
   {"type":"ABS",                    "desc":"Abstract"                        },
   {"type":"BLC",                    "desc":"Blood Code"                       ,"repeats":true},
   {"type":"RMI",                    "desc":"Risk Management Incident"        }]},
 "VISIT_ORUR30":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_ORUR31":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_ORUR32":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_OULR21":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_OULR22":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_OULR23":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_OULR24":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PATIENT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PATIENT_CSUC10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PATIENT_CSUC11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PATIENT_CSUC12":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PATIENT_PATIENT_RESULT":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PATIENT_QUERY_RESPONSE":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"AL1", "desc":"Patient Allergy Information" ,"req":true ,"repeats":true},
   {"type":"PV1", "desc":"Patient Visit"               ,"req":true},
   {"type":"PV2", "desc":"Additional Information"     }]},
 "VISIT_PATIENT_QUERY_RESPONSE_RSPZ90":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PEXP07":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_PEXP08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RPAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RPAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RPAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RPAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RQAI08":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RQAI09":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RQAI10":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VISIT_RQAI11":{
  "desc":"",
  "type":"segment_group",
  "children":[
   {"type":"PV1", "desc":"Patient Visit"          ,"req":true},
   {"type":"PV2", "desc":"Additional Information"}]},
 "VR":{
  "desc":"Value Range",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"First Data Code Value"},
   {"type":"ST", "desc":"Last Data Code Value" }]},
 "VTQ":{
  "desc":"Virtual Table Query Request",
  "type":"segment",
  "children":[
   {"type":"ST",  "desc":"Query Tag"                 },
   {"type":"ID",  "desc":"Query/response Format Code" ,"req":true},
   {"type":"CE",  "desc":"Vt Query Name"              ,"req":true},
   {"type":"CE",  "desc":"Virtual Table Name"         ,"req":true},
   {"type":"QSC", "desc":"Selection Criteria"         ,"repeats":true}]},
 "VXQV01":{
  "desc":"Query for vaccination record",
  "type":"message",
  "children":[
   {"type":"MSH", "desc":"Message Header"              ,"req":true},
   {"type":"SFT", "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD", "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF", "desc":"Original Style Query Filter"}]},
 "VXRV03":{
  "desc":"Vaccination record response",
  "type":"message",
  "children":[
   {"type":"MSH",                  "desc":"Message Header"                   ,"req":true},
   {"type":"MSA",                  "desc":"Message Acknowledgment"           ,"req":true},
   {"type":"SFT",                  "desc":"Software Segment"                 ,"repeats":true},
   {"type":"QRD",                  "desc":"Style Query Definition"           ,"req":true},
   {"type":"QRF",                  "desc":"Original Style Query Filter"     },
   {"type":"PID",                  "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",                  "desc":"Patient Additional Demographic"  },
   {"type":"NK1",                  "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PATIENT_VISIT_VXRV03", "desc":""                                },
   {"type":"GT1",                  "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_VXRV03",     "desc":""                                 ,"repeats":true},
   {"type":"ORDER_VXRV03",         "desc":""                                 ,"repeats":true}]},
 "VXUV04":{
  "desc":"Unsolicited vaccination record update",
  "type":"message",
  "children":[
   {"type":"MSH",              "desc":"Message Header"                   ,"req":true},
   {"type":"SFT",              "desc":"Software Segment"                 ,"repeats":true},
   {"type":"PID",              "desc":"Patient Identification"           ,"req":true},
   {"type":"PD1",              "desc":"Patient Additional Demographic"  },
   {"type":"NK1",              "desc":"Next Of Kin / Associated Parties" ,"repeats":true},
   {"type":"PATIENT_VXUV04",   "desc":""                                },
   {"type":"GT1",              "desc":"Guarantor"                        ,"repeats":true},
   {"type":"INSURANCE_VXUV04", "desc":""                                 ,"repeats":true},
   {"type":"ORDER_VXUV04",     "desc":""                                 ,"repeats":true}]},
 "VXXV02":{
  "desc":"Response to vaccination query returning multiple PID matches",
  "type":"message",
  "children":[
   {"type":"MSH",            "desc":"Message Header"              ,"req":true},
   {"type":"MSA",            "desc":"Message Acknowledgment"      ,"req":true},
   {"type":"SFT",            "desc":"Software Segment"            ,"repeats":true},
   {"type":"QRD",            "desc":"Style Query Definition"      ,"req":true},
   {"type":"QRF",            "desc":"Original Style Query Filter"},
   {"type":"PATIENT_VXXV02", "desc":""                            ,"req":true ,"repeats":true}]},
 "Varies":{
  "desc":"None",
  "type":"composite"
 },
 "XAD":{
  "desc":"Extended Address",
  "type":"composite",
  "children":[
   {"type":"SAD", "desc":"Street Address"              },
   {"type":"ST",  "desc":"Other Designation"           },
   {"type":"ST",  "desc":"City"                        },
   {"type":"ST",  "desc":"State Or Province"           },
   {"type":"ST",  "desc":"Zip Or Postal Code"          },
   {"type":"ID",  "desc":"Country"                     },
   {"type":"ID",  "desc":"Address Type"                },
   {"type":"ST",  "desc":"Other Geographic Designation"},
   {"type":"IS",  "desc":"County/parish Code"          },
   {"type":"IS",  "desc":"Census Tract"                },
   {"type":"ID",  "desc":"Address Representation Code" },
   {"type":"DR",  "desc":"Address Validity Range"      },
   {"type":"TS",  "desc":"Effective Date"              },
   {"type":"TS",  "desc":"Expiration Date"             }]},
 "XCN":{
  "desc":"Extended Composite ID Number And Name For Persons",
  "type":"composite",
  "children":[
   {"type":"ST",  "desc":"ID Number"                                         },
   {"type":"FN",  "desc":"Family Name"                                       },
   {"type":"ST",  "desc":"Given Name"                                        },
   {"type":"ST",  "desc":"Second And Further Given Names Or Initials Thereof"},
   {"type":"ST",  "desc":"Suffix (e.g., Jr Or III)"                          },
   {"type":"ST",  "desc":"Prefix (e.g., Dr)"                                 },
   {"type":"IS",  "desc":"Degree (e.g., Md)"                                 },
   {"type":"IS",  "desc":"Source Table"                                      },
   {"type":"HD",  "desc":"Assigning Authority"                               },
   {"type":"ID",  "desc":"Name Type Code"                                    },
   {"type":"ST",  "desc":"Identifier Check Digit"                            },
   {"type":"ID",  "desc":"Check Digit Scheme"                                },
   {"type":"ID",  "desc":"Identifier Type Code"                              },
   {"type":"HD",  "desc":"Assigning Facility"                                },
   {"type":"ID",  "desc":"Name Representation Code"                          },
   {"type":"CE",  "desc":"Name Context"                                      },
   {"type":"DR",  "desc":"Name Validity Range"                               },
   {"type":"ID",  "desc":"Name Assembly Order"                               },
   {"type":"TS",  "desc":"Effective Date"                                    },
   {"type":"TS",  "desc":"Expiration Date"                                   },
   {"type":"ST",  "desc":"Professional Suffix"                               },
   {"type":"CWE", "desc":"Assigning Jurisdiction"                            },
   {"type":"CWE", "desc":"Assigning Agency Or Department"                    }]},
 "XON":{
  "desc":"Extended Composite Name And Identification Number For Organizations",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Organization Name"          },
   {"type":"IS", "desc":"Organization Name Type Code"},
   {"type":"NM", "desc":"ID Number"                  },
   {"type":"NM", "desc":"Check Digit"                },
   {"type":"ID", "desc":"Check Digit Scheme"         },
   {"type":"HD", "desc":"Assigning Authority"        },
   {"type":"ID", "desc":"Identifier Type Code"       },
   {"type":"HD", "desc":"Assigning Facility"         },
   {"type":"ID", "desc":"Name Representation Code"   },
   {"type":"ST", "desc":"Organization Identifier"    }]},
 "XPN":{
  "desc":"Extended Person Name",
  "type":"composite",
  "children":[
   {"type":"FN", "desc":"Family Name"                                       },
   {"type":"ST", "desc":"Given Name"                                        },
   {"type":"ST", "desc":"Second And Further Given Names Or Initials Thereof"},
   {"type":"ST", "desc":"Suffix (e.g., Jr Or III)"                          },
   {"type":"ST", "desc":"Prefix (e.g., Dr)"                                 },
   {"type":"IS", "desc":"Degree (e.g., Md)"                                 },
   {"type":"ID", "desc":"Name Type Code"                                    },
   {"type":"ID", "desc":"Name Representation Code"                          },
   {"type":"CE", "desc":"Name Context"                                      },
   {"type":"DR", "desc":"Name Validity Range"                               },
   {"type":"ID", "desc":"Name Assembly Order"                               },
   {"type":"TS", "desc":"Effective Date"                                    },
   {"type":"TS", "desc":"Expiration Date"                                   },
   {"type":"ST", "desc":"Professional Suffix"                               }]},
 "XTN":{
  "desc":"Extended Telecommunication Number",
  "type":"composite",
  "children":[
   {"type":"ST", "desc":"Telephone Number"                },
   {"type":"ID", "desc":"Telecommunication Use Code"      },
   {"type":"ID", "desc":"Telecommunication Equipment Type"},
   {"type":"ST", "desc":"Email Address"                   },
   {"type":"NM", "desc":"Country Code"                    },
   {"type":"NM", "desc":"Area/city Code"                  },
   {"type":"NM", "desc":"Local Number"                    },
   {"type":"NM", "desc":"Extension"                       },
   {"type":"ST", "desc":"Any Text"                        },
   {"type":"ST", "desc":"Extension Prefix"                },
   {"type":"ST", "desc":"Speed Dial Code"                 },
   {"type":"ST", "desc":"Unformatted Telephone Number"    }]},
 "ZL7":{
  "desc":"(Proposed Example Only)",
  "type":"segment"
 }},
"matching":[
 {"rule":"ACK"     , "name":"ACK"   },
 {"rule":"ADR^A19*", "name":"ADRA19"},
 {"rule":"ADT^A01*", "name":"ADTA01"},
 {"rule":"ADT^A02*", "name":"ADTA02"},
 {"rule":"ADT^A03*", "name":"ADTA03"},
 {"rule":"ADT^A04*", "name":"ADTA04"},
 {"rule":"ADT^A05*", "name":"ADTA05"},
 {"rule":"ADT^A06*", "name":"ADTA06"},
 {"rule":"ADT^A07*", "name":"ADTA07"},
 {"rule":"ADT^A08*", "name":"ADTA08"},
 {"rule":"ADT^A09*", "name":"ADTA09"},
 {"rule":"ADT^A10*", "name":"ADTA10"},
 {"rule":"ADT^A11*", "name":"ADTA11"},
 {"rule":"ADT^A12*", "name":"ADTA12"},
 {"rule":"ADT^A13*", "name":"ADTA13"},
 {"rule":"ADT^A14*", "name":"ADTA14"},
 {"rule":"ADT^A15*", "name":"ADTA15"},
 {"rule":"ADT^A16*", "name":"ADTA16"},
 {"rule":"ADT^A17*", "name":"ADTA17"},
 {"rule":"ADT^A18*", "name":"ADTA18"},
 {"rule":"ADT^A20*", "name":"ADTA20"},
 {"rule":"ADT^A21*", "name":"ADTA21"},
 {"rule":"ADT^A22*", "name":"ADTA22"},
 {"rule":"ADT^A23*", "name":"ADTA23"},
 {"rule":"ADT^A24*", "name":"ADTA24"},
 {"rule":"ADT^A25*", "name":"ADTA25"},
 {"rule":"ADT^A26*", "name":"ADTA26"},
 {"rule":"ADT^A27*", "name":"ADTA27"},
 {"rule":"ADT^A28*", "name":"ADTA28"},
 {"rule":"ADT^A29*", "name":"ADTA29"},
 {"rule":"ADT^A30*", "name":"ADTA30"},
 {"rule":"ADT^A31*", "name":"ADTA31"},
 {"rule":"ADT^A32*", "name":"ADTA32"},
 {"rule":"ADT^A33*", "name":"ADTA33"},
 {"rule":"ADT^A34*", "name":"ADTA34"},
 {"rule":"ADT^A35*", "name":"ADTA35"},
 {"rule":"ADT^A36*", "name":"ADTA36"},
 {"rule":"ADT^A37*", "name":"ADTA37"},
 {"rule":"ADT^A38*", "name":"ADTA38"},
 {"rule":"ADT^A39*", "name":"ADTA39"},
 {"rule":"ADT^A40*", "name":"ADTA40"},
 {"rule":"ADT^A41*", "name":"ADTA41"},
 {"rule":"ADT^A42*", "name":"ADTA42"},
 {"rule":"ADT^A43*", "name":"ADTA43"},
 {"rule":"ADT^A44*", "name":"ADTA44"},
 {"rule":"ADT^A45*", "name":"ADTA45"},
 {"rule":"ADT^A46*", "name":"ADTA46"},
 {"rule":"ADT^A47*", "name":"ADTA47"},
 {"rule":"ADT^A48*", "name":"ADTA48"},
 {"rule":"ADT^A49*", "name":"ADTA49"},
 {"rule":"ADT^A50*", "name":"ADTA50"},
 {"rule":"ADT^A51*", "name":"ADTA51"},
 {"rule":"ADT^A52*", "name":"ADTA52"},
 {"rule":"ADT^A53*", "name":"ADTA53"},
 {"rule":"ADT^A54*", "name":"ADTA54"},
 {"rule":"ADT^A55*", "name":"ADTA55"},
 {"rule":"ADT^A60*", "name":"ADTA60"},
 {"rule":"ADT^A61*", "name":"ADTA61"},
 {"rule":"ADT^A62*", "name":"ADTA62"},
 {"rule":"BAR^P01*", "name":"BARP01"},
 {"rule":"BAR^P02*", "name":"BARP02"},
 {"rule":"BAR^P05*", "name":"BARP05"},
 {"rule":"BAR^P06*", "name":"BARP06"},
 {"rule":"BAR^P10*", "name":"BARP10"},
 {"rule":"BAR^P12*", "name":"BARP12"},
 {"rule":"BPS^O29*", "name":"BPSO29"},
 {"rule":"BRP^O30*", "name":"BRPO30"},
 {"rule":"BRT^O32*", "name":"BRTO32"},
 {"rule":"BTS^O31*", "name":"BTSO31"},
 {"rule":"CRM^C01*", "name":"CRMC01"},
 {"rule":"CRM^C02*", "name":"CRMC02"},
 {"rule":"CRM^C03*", "name":"CRMC03"},
 {"rule":"CRM^C04*", "name":"CRMC04"},
 {"rule":"CRM^C05*", "name":"CRMC05"},
 {"rule":"CRM^C06*", "name":"CRMC06"},
 {"rule":"CRM^C07*", "name":"CRMC07"},
 {"rule":"CRM^C08*", "name":"CRMC08"},
 {"rule":"CSU^C09*", "name":"CSUC09"},
 {"rule":"CSU^C10*", "name":"CSUC10"},
 {"rule":"CSU^C11*", "name":"CSUC11"},
 {"rule":"CSU^C12*", "name":"CSUC12"},
 {"rule":"DFT^P03*", "name":"DFTP03"},
 {"rule":"DFT^P11*", "name":"DFTP11"},
 {"rule":"DOC^T12*", "name":"DOCT12"},
 {"rule":"EAC^U07*", "name":"EACU07"},
 {"rule":"EAN^U09*", "name":"EANU09"},
 {"rule":"EAR^U08*", "name":"EARU08"},
 {"rule":"ESR^U02*", "name":"ESRU02"},
 {"rule":"ESU^U01*", "name":"ESUU01"},
 {"rule":"INR^U06*", "name":"INRU06"},
 {"rule":"INU^U05*", "name":"INUU05"},
 {"rule":"LSU^U12*", "name":"LSUU12"},
 {"rule":"LSU^U13*", "name":"LSUU13"},
 {"rule":"MDM^T01*", "name":"MDMT01"},
 {"rule":"MDM^T02*", "name":"MDMT02"},
 {"rule":"MDM^T03*", "name":"MDMT03"},
 {"rule":"MDM^T04*", "name":"MDMT04"},
 {"rule":"MDM^T05*", "name":"MDMT05"},
 {"rule":"MDM^T06*", "name":"MDMT06"},
 {"rule":"MDM^T07*", "name":"MDMT07"},
 {"rule":"MDM^T08*", "name":"MDMT08"},
 {"rule":"MDM^T09*", "name":"MDMT09"},
 {"rule":"MDM^T10*", "name":"MDMT10"},
 {"rule":"MDM^T11*", "name":"MDMT11"},
 {"rule":"MFK^M01*", "name":"MFKM01"},
 {"rule":"MFK^M02*", "name":"MFKM02"},
 {"rule":"MFK^M03*", "name":"MFKM03"},
 {"rule":"MFK^M04*", "name":"MFKM04"},
 {"rule":"MFK^M05*", "name":"MFKM05"},
 {"rule":"MFK^M06*", "name":"MFKM06"},
 {"rule":"MFK^M07*", "name":"MFKM07"},
 {"rule":"MFK^M08*", "name":"MFKM08"},
 {"rule":"MFK^M09*", "name":"MFKM09"},
 {"rule":"MFK^M10*", "name":"MFKM10"},
 {"rule":"MFK^M11*", "name":"MFKM11"},
 {"rule":"MFK^M12*", "name":"MFKM12"},
 {"rule":"MFK^M13*", "name":"MFKM13"},
 {"rule":"MFK^M14*", "name":"MFKM14"},
 {"rule":"MFK^M15*", "name":"MFKM15"},
 {"rule":"MFN^M01*", "name":"MFNM01"},
 {"rule":"MFN^M02*", "name":"MFNM02"},
 {"rule":"MFN^M03*", "name":"MFNM03"},
 {"rule":"MFN^M04*", "name":"MFNM04"},
 {"rule":"MFN^M05*", "name":"MFNM05"},
 {"rule":"MFN^M06*", "name":"MFNM06"},
 {"rule":"MFN^M07*", "name":"MFNM07"},
 {"rule":"MFN^M08*", "name":"MFNM08"},
 {"rule":"MFN^M09*", "name":"MFNM09"},
 {"rule":"MFN^M10*", "name":"MFNM10"},
 {"rule":"MFN^M11*", "name":"MFNM11"},
 {"rule":"MFN^M12*", "name":"MFNM12"},
 {"rule":"MFN^M13*", "name":"MFNM13"},
 {"rule":"MFN^M14*", "name":"MFNM14"},
 {"rule":"MFN^M15*", "name":"MFNM15"},
 {"rule":"NMD^N02*", "name":"NMDN02"},
 {"rule":"NMQ^N01*", "name":"NMQN01"},
 {"rule":"NMR^N01*", "name":"NMRN01"},
 {"rule":"OMB^O27*", "name":"OMBO27"},
 {"rule":"OMD^O03*", "name":"OMDO03"},
 {"rule":"OMG^O19*", "name":"OMGO19"},
 {"rule":"OMI^O23*", "name":"OMIO23"},
 {"rule":"OML^O21*", "name":"OMLO21"},
 {"rule":"OML^O33*", "name":"OMLO33"},
 {"rule":"OML^O35*", "name":"OMLO35"},
 {"rule":"OMN^O07*", "name":"OMNO07"},
 {"rule":"OMP^O09*", "name":"OMPO09"},
 {"rule":"OMS^O05*", "name":"OMSO05"},
 {"rule":"ORB^O28*", "name":"ORBO28"},
 {"rule":"ORD^O04*", "name":"ORDO04"},
 {"rule":"ORF^R04*", "name":"ORFR04"},
 {"rule":"ORG^O20*", "name":"ORGO20"},
 {"rule":"ORI^O24*", "name":"ORIO24"},
 {"rule":"ORL^O22*", "name":"ORLO22"},
 {"rule":"ORL^O34*", "name":"ORLO34"},
 {"rule":"ORL^O36*", "name":"ORLO36"},
 {"rule":"ORM^O01*", "name":"ORMO01"},
 {"rule":"ORN^O08*", "name":"ORNO08"},
 {"rule":"ORP^O10*", "name":"ORPO10"},
 {"rule":"ORR^O02*", "name":"ORRO02"},
 {"rule":"ORS^O06*", "name":"ORSO06"},
 {"rule":"ORU^R01*", "name":"ORUR01"},
 {"rule":"ORU^R30*", "name":"ORUR30"},
 {"rule":"ORU^R31*", "name":"ORUR31"},
 {"rule":"ORU^R32*", "name":"ORUR32"},
 {"rule":"OSQ^Q06*", "name":"OSQQ06"},
 {"rule":"OSR^Q06*", "name":"OSRQ06"},
 {"rule":"OUL^R21*", "name":"OULR21"},
 {"rule":"OUL^R22*", "name":"OULR22"},
 {"rule":"OUL^R23*", "name":"OULR23"},
 {"rule":"OUL^R24*", "name":"OULR24"},
 {"rule":"PEX^P07*", "name":"PEXP07"},
 {"rule":"PEX^P08*", "name":"PEXP08"},
 {"rule":"PGL^PC6*", "name":"PGLPC6"},
 {"rule":"PGL^PC7*", "name":"PGLPC7"},
 {"rule":"PGL^PC8*", "name":"PGLPC8"},
 {"rule":"PMU^B01*", "name":"PMUB01"},
 {"rule":"PMU^B02*", "name":"PMUB02"},
 {"rule":"PMU^B03*", "name":"PMUB03"},
 {"rule":"PMU^B04*", "name":"PMUB04"},
 {"rule":"PMU^B05*", "name":"PMUB05"},
 {"rule":"PMU^B06*", "name":"PMUB06"},
 {"rule":"PMU^B07*", "name":"PMUB07"},
 {"rule":"PMU^B08*", "name":"PMUB08"},
 {"rule":"PPG^PCG*", "name":"PPGPCG"},
 {"rule":"PPG^PCH*", "name":"PPGPCH"},
 {"rule":"PPG^PCJ*", "name":"PPGPCJ"},
 {"rule":"PPP^PCB*", "name":"PPPPCB"},
 {"rule":"PPP^PCC*", "name":"PPPPCC"},
 {"rule":"PPP^PCD*", "name":"PPPPCD"},
 {"rule":"PPR^PC1*", "name":"PPRPC1"},
 {"rule":"PPR^PC2*", "name":"PPRPC2"},
 {"rule":"PPR^PC3*", "name":"PPRPC3"},
 {"rule":"PPT^PCL*", "name":"PPTPCL"},
 {"rule":"PPV^PCA*", "name":"PPVPCA"},
 {"rule":"PRR^PC5*", "name":"PRRPC5"},
 {"rule":"PTR^PCF*", "name":"PTRPCF"},
 {"rule":"QBP^Q11*", "name":"QBPQ11"},
 {"rule":"QBP^Q13*", "name":"QBPQ13"},
 {"rule":"QBP^Q15*", "name":"QBPQ15"},
 {"rule":"QBP^Q21*", "name":"QBPQ21"},
 {"rule":"QBP^Q22*", "name":"QBPQ22"},
 {"rule":"QBP^Q23*", "name":"QBPQ23"},
 {"rule":"QBP^Q24*", "name":"QBPQ24"},
 {"rule":"QBP^Q25*", "name":"QBPQ25"},
 {"rule":"QBP^Q31*", "name":"QBPQ31"},
 {"rule":"QBP^Z73*", "name":"QBPZ73"},
 {"rule":"QBP^Z75*", "name":"QBPZ75"},
 {"rule":"QBP^Z77*", "name":"QBPZ77"},
 {"rule":"QBP^Z79*", "name":"QBPZ79"},
 {"rule":"QBP^Z81*", "name":"QBPZ81"},
 {"rule":"QBP^Z85*", "name":"QBPZ85"},
 {"rule":"QBP^Z87*", "name":"QBPZ87"},
 {"rule":"QBP^Z89*", "name":"QBPZ89"},
 {"rule":"QBP^Z91*", "name":"QBPZ91"},
 {"rule":"QBP^Z93*", "name":"QBPZ93"},
 {"rule":"QBP^Z95*", "name":"QBPZ95"},
 {"rule":"QBP^Z97*", "name":"QBPZ97"},
 {"rule":"QBP^Z99*", "name":"QBPZ99"},
 {"rule":"QCN^J01*", "name":"QCNJ01"},
 {"rule":"QCN^J02*", "name":"QCNJ02"},
 {"rule":"QRY^A19*", "name":"QRYA19"},
 {"rule":"QRY^PC4*", "name":"QRYPC4"},
 {"rule":"QRY^PC9*", "name":"QRYPC9"},
 {"rule":"QRY^PCE*", "name":"QRYPCE"},
 {"rule":"QRY^PCK*", "name":"QRYPCK"},
 {"rule":"QRY^Q26*", "name":"QRYQ26"},
 {"rule":"QRY^Q27*", "name":"QRYQ27"},
 {"rule":"QRY^Q28*", "name":"QRYQ28"},
 {"rule":"QRY^Q29*", "name":"QRYQ29"},
 {"rule":"QRY^Q30*", "name":"QRYQ30"},
 {"rule":"QRY^R02*", "name":"QRYR02"},
 {"rule":"QRY^T12*", "name":"QRYT12"},
 {"rule":"QSB^Q16*", "name":"QSBQ16"},
 {"rule":"QSB^Z83*", "name":"QSBZ83"},
 {"rule":"QVR^Q17*", "name":"QVRQ17"},
 {"rule":"RAR^RAR*", "name":"RARRAR"},
 {"rule":"RAS^O17*", "name":"RASO17"},
 {"rule":"RCI^I05*", "name":"RCII05"},
 {"rule":"RCL^I06*", "name":"RCLI06"},
 {"rule":"RDE^O11*", "name":"RDEO11"},
 {"rule":"RDE^O25*", "name":"RDEO25"},
 {"rule":"RDR^RDR*", "name":"RDRRDR"},
 {"rule":"RDS^O13*", "name":"RDSO13"},
 {"rule":"RDY^K15*", "name":"RDYK15"},
 {"rule":"RDY^Z98*", "name":"RDYZ98"},
 {"rule":"REF^I12*", "name":"REFI12"},
 {"rule":"REF^I13*", "name":"REFI13"},
 {"rule":"REF^I14*", "name":"REFI14"},
 {"rule":"REF^I15*", "name":"REFI15"},
 {"rule":"RER^RER*", "name":"RERRER"},
 {"rule":"RGR^RGR*", "name":"RGRRGR"},
 {"rule":"RGV^O15*", "name":"RGVO15"},
 {"rule":"ROR^ROR*", "name":"RORROR"},
 {"rule":"RPA^I08*", "name":"RPAI08"},
 {"rule":"RPA^I09*", "name":"RPAI09"},
 {"rule":"RPA^I10*", "name":"RPAI10"},
 {"rule":"RPA^I11*", "name":"RPAI11"},
 {"rule":"RPI^I01*", "name":"RPII01"},
 {"rule":"RPI^I04*", "name":"RPII04"},
 {"rule":"RPL^I02*", "name":"RPLI02"},
 {"rule":"RPR^I03*", "name":"RPRI03"},
 {"rule":"RQA^I08*", "name":"RQAI08"},
 {"rule":"RQA^I09*", "name":"RQAI09"},
 {"rule":"RQA^I10*", "name":"RQAI10"},
 {"rule":"RQA^I11*", "name":"RQAI11"},
 {"rule":"RQC^I05*", "name":"RQCI05"},
 {"rule":"RQC^I06*", "name":"RQCI06"},
 {"rule":"RQI^I01*", "name":"RQII01"},
 {"rule":"RQI^I02*", "name":"RQII02"},
 {"rule":"RQI^I03*", "name":"RQII03"},
 {"rule":"RQI^I07*", "name":"RQII07"},
 {"rule":"RQP^I04*", "name":"RQPI04"},
 {"rule":"RRA^O18*", "name":"RRAO18"},
 {"rule":"RRD^O14*", "name":"RRDO14"},
 {"rule":"RRE^O12*", "name":"RREO12"},
 {"rule":"RRE^O26*", "name":"RREO26"},
 {"rule":"RRG^O16*", "name":"RRGO16"},
 {"rule":"RRI^I12*", "name":"RRII12"},
 {"rule":"RRI^I13*", "name":"RRII13"},
 {"rule":"RRI^I14*", "name":"RRII14"},
 {"rule":"RRI^I15*", "name":"RRII15"},
 {"rule":"RSP^K11*", "name":"RSPK11"},
 {"rule":"RSP^K21*", "name":"RSPK21"},
 {"rule":"RSP^K22*", "name":"RSPK22"},
 {"rule":"RSP^K23*", "name":"RSPK23"},
 {"rule":"RSP^K24*", "name":"RSPK24"},
 {"rule":"RSP^K25*", "name":"RSPK25"},
 {"rule":"RSP^K31*", "name":"RSPK31"},
 {"rule":"RSP^Z80*", "name":"RSPZ80"},
 {"rule":"RSP^Z82*", "name":"RSPZ82"},
 {"rule":"RSP^Z84*", "name":"RSPZ84"},
 {"rule":"RSP^Z86*", "name":"RSPZ86"},
 {"rule":"RSP^Z88*", "name":"RSPZ88"},
 {"rule":"RSP^Z90*", "name":"RSPZ90"},
 {"rule":"RTB^K13*", "name":"RTBK13"},
 {"rule":"RTB^Z74*", "name":"RTBZ74"},
 {"rule":"RTB^Z76*", "name":"RTBZ76"},
 {"rule":"RTB^Z78*", "name":"RTBZ78"},
 {"rule":"RTB^Z92*", "name":"RTBZ92"},
 {"rule":"RTB^Z94*", "name":"RTBZ94"},
 {"rule":"RTB^Z96*", "name":"RTBZ96"},
 {"rule":"SIU^S12*", "name":"SIUS12"},
 {"rule":"SIU^S13*", "name":"SIUS13"},
 {"rule":"SIU^S14*", "name":"SIUS14"},
 {"rule":"SIU^S15*", "name":"SIUS15"},
 {"rule":"SIU^S16*", "name":"SIUS16"},
 {"rule":"SIU^S17*", "name":"SIUS17"},
 {"rule":"SIU^S18*", "name":"SIUS18"},
 {"rule":"SIU^S19*", "name":"SIUS19"},
 {"rule":"SIU^S20*", "name":"SIUS20"},
 {"rule":"SIU^S21*", "name":"SIUS21"},
 {"rule":"SIU^S22*", "name":"SIUS22"},
 {"rule":"SIU^S23*", "name":"SIUS23"},
 {"rule":"SIU^S24*", "name":"SIUS24"},
 {"rule":"SIU^S26*", "name":"SIUS26"},
 {"rule":"SQM^S25*", "name":"SQMS25"},
 {"rule":"SQR^S25*", "name":"SQRS25"},
 {"rule":"SRM^S01*", "name":"SRMS01"},
 {"rule":"SRM^S02*", "name":"SRMS02"},
 {"rule":"SRM^S03*", "name":"SRMS03"},
 {"rule":"SRM^S04*", "name":"SRMS04"},
 {"rule":"SRM^S05*", "name":"SRMS05"},
 {"rule":"SRM^S06*", "name":"SRMS06"},
 {"rule":"SRM^S07*", "name":"SRMS07"},
 {"rule":"SRM^S08*", "name":"SRMS08"},
 {"rule":"SRM^S09*", "name":"SRMS09"},
 {"rule":"SRM^S10*", "name":"SRMS10"},
 {"rule":"SRM^S11*", "name":"SRMS11"},
 {"rule":"SRR^S01*", "name":"SRRS01"},
 {"rule":"SRR^S02*", "name":"SRRS02"},
 {"rule":"SRR^S03*", "name":"SRRS03"},
 {"rule":"SRR^S04*", "name":"SRRS04"},
 {"rule":"SRR^S05*", "name":"SRRS05"},
 {"rule":"SRR^S06*", "name":"SRRS06"},
 {"rule":"SRR^S07*", "name":"SRRS07"},
 {"rule":"SRR^S08*", "name":"SRRS08"},
 {"rule":"SRR^S09*", "name":"SRRS09"},
 {"rule":"SRR^S10*", "name":"SRRS10"},
 {"rule":"SRR^S11*", "name":"SRRS11"},
 {"rule":"SSR^U04*", "name":"SSRU04"},
 {"rule":"SSU^U03*", "name":"SSUU03"},
 {"rule":"SUR^P09*", "name":"SURP09"},
 {"rule":"TCU^U10*", "name":"TCUU10"},
 {"rule":"TCU^U11*", "name":"TCUU11"},
 {"rule":"VXQ^V01*", "name":"VXQV01"},
 {"rule":"VXR^V03*", "name":"VXRV03"},
 {"rule":"VXU^V04*", "name":"VXUV04"},
 {"rule":"VXX^V02*", "name":"VXXV02"}]
}
