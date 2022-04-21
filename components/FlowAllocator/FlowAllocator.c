/*Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

/* RINA includes. */
#include "common.h"
#include "configSensor.h"
#include "configRINA.h"
#include "Ribd.h"
#include "Enrollment.h"
#include "EnrollmentInformationMessage.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "Rib.h"
#include "SerdesMsg.h"
#include "FlowAllocator.h"
#include "pidm.h"
#include "RINA_API.h"
#include "IPCP.h"

#include "esp_log.h"

/* Create_Request: handle the request send by other IPCP. Consults the local
 * directory Forwarding Table. It is to me, create a FAI*/

BaseType_t xFlowAllocatorInit()
{
    flowAllocator_t *pxFlowAllocator;
    pxFlowAllocator = pvPortMalloc(sizeof(*pxFlowAllocator));

    /* Create object in the Rib*/
    pxRibCreateObject("/fa/flows", 0, "Flow_Allocator", "Flow", FLOW_ALLOCATOR);

    /*Init List*/
    vListInitialise(&pxFlowAllocator->xFlowAllocatorInstances);

    return pdTRUE;
}

/**/
static flow_t *prvFlowAllocatorNewFlow(flowAllocateHandle_t *pxFlowRequest)
{
    flow_t *pxFlow;
    pxFlow = pvPortMalloc(sizeof(*pxFlow));
    pxFlow->pxQosSpec = pvPortMalloc(sizeof(qosSpec_t *));
    pxFlow->pxQosSpec->pxFlowSpec = pvPortMalloc(sizeof(struct flowSpec_t*));

    pxFlow->pxSourceInfo = pxFlowRequest->pxLocal;
    pxFlow->pxDestInfo = pxFlowRequest->pxRemote;
    pxFlow->ulHopCount = 3;
    pxFlow->ulMaxCreateFlowRetries = 1;
    pxFlow->eState = eALLOCATION_IN_PROGRESS;
    pxFlow->xSourceAddress = LOCAL_ADDRESS;


    /* TODO: Found the most suitable QoScube for the pxFlowRequest->pxFspec
     * Now, it use a default cube to test */
    pxFlow->pxQosSpec->xQosId = QoS_CUBE_ID;
    pxFlow->pxQosSpec->pcQosName = QoS_CUBE_NAME;
    pxFlow->pxQosSpec->pxFlowSpec->xPartialDelivery = QoS_CUBE_PARTIAL_DELIVERY;
    pxFlow->pxQosSpec->pxFlowSpec->xOrderedDelivery = QoS_CUBE_ORDERED_DELIVERY;

    /* Fulfill the DTP_config and the DTCP_config based on the QoSCube*/
    pxFlow->pxDtpConfig = pvPortMalloc((sizeof(dtpConfig_t *)));
    pxFlow->pxDtpConfig->xDtcpPresent = DTP_DTCP_PRESENT;
    pxFlow->pxDtpConfig->xInitialATimer = DTP_INITIAL_A_TIMER;

    pxFlow->pxDtpConfig->pxDtpPolicySet = pvPortMalloc(sizeof(policy_t *));
    pxFlow->pxDtpConfig->pxDtpPolicySet->pcPolicyName = DTP_POLICY_SET_NAME;
    pxFlow->pxDtpConfig->pxDtpPolicySet->pcPolicyVersion = DTP_POLICY_SET_VERSION;

    // By the moment the DTCP is not implemented yet so we are using DTP_DTCP_PRESENT = pdFALSE
    pxFlow->pxDtcpConfig = NULL;
    if (pxFlow->pxDtpConfig->xDtcpPresent == pdTRUE)
    {
        pxFlow->pxDtcpConfig = pvPortMalloc(sizeof(struct dtcpConfig_t *));
    }

    return pxFlow;
}

/*Allocate_Request: Handle the request send by the application
 * if it is well-formed, create a new FlowAllocator-Instance*/
BaseType_t xFlowAllocatorFlowRequest(ipcpInstance_t *pxNormalInstance, portId_t xPortId, flowAllocateHandle_t *pxFlowRequest)
{
    
    flow_t *pxFlow;
    cepId_t xCepSourceId;
    flowAllocatorInstace_t *pxFlowAllocatorInstance;

    /* Create a flow object and fill using the event FlowRequest */
    pxFlow = prvFlowAllocatorNewFlow(pxFlowRequest);

    /* Create a FAI and fill the struct properly*/
    pxFlowAllocatorInstance = pvPortMalloc(sizeof(*pxFlowAllocatorInstance));

    pxFlowAllocatorInstance->eFaiState = eFAI_NONE;
    pxFlowAllocatorInstance->xPortId = xPortId;

    /* Request to DFT the Next Hop, at the moment request to EnrollmmentTask */
    pxFlow->xRemoteAddress = xEnrollmentGetNeighborAddress(pxFlow->pxDestInfo->pcProcessName);
    
    pxFlow->xSourcePortId = xPortId;


    if (pxFlow->xRemoteAddress == -1)
    {
        ESP_LOGE(TAG_FA, "Error to get Next Hop");
        return pdFALSE;
    }

    /* Call EFCP to create an EFCP instance following the EFCP Config */
    xCepSourceId = pxNormalInstance->pxOps->connectionCreate(pxNormalInstance->pxData, xPortId,
                                                       LOCAL_ADDRESS, pxFlow->xRemoteAddress, pxFlow->pxQosSpec->xQosId,
                                                       pxFlow->pxDtpConfig, pxFlow->pxDtcpConfig);
    if (xCepSourceId == -1)
    {
        ESP_LOGE(TAG_FA, "CepId was not create properly");
        return pdFALSE;
    }

    /* Fill the Flow connectionId */
    pxFlow->pxConnectionId = pvPortMalloc(sizeof(connectionId_t*));
    pxFlow->pxConnectionId->xSource = xCepSourceId;
    pxFlow->pxConnectionId->xQosId = pxFlow->pxQosSpec->xQosId;

    /* Send the flow message to the neighbor */
    //Serialize the pxFLow Struct into FlowMsg and Encode the FlowMsg as obj_value

    serObjectValue_t *pxObjVal = pxSerdesMsgFlowEncode(pxFlow);

    //Send using the ribd_send_req M_Create
    if (!xRibdSendRequest("Flow_Allocator", "/fa/flows", -1, M_START, xPortId, pxObjVal))
        {
                ESP_LOGE(TAG_FA, "It was a problem to sen the request");
                return pdFALSE;
        }

    return pdTRUE;

}